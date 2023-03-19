#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "help.h"
#include "exec-cmd.h"

#define SIMILARITY_FLOOR 7
#define SIMILAR_ENOUGH(x) ((x) < SIMILARITY_FLOOR)

#define RUN_SETUP (1<<0)
#define RUN_SETUP_GENTLY (1<<1)
#define USE_PAGER (1<<2)

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
/*
 * require working tree to be present -- anything uses this needs
 * RUN_SETUP for reading from the configuration file.
 */
#define NEED_WORK_TREE (1<<3)
#define DELAY_PAGER_CONFIG (1<<4)
#define NO_PARSEOPT (1<<5) /* parse-options is not used */

#define ARRAY_SIZE(x) sizeof(x) / sizeof((x)[0])

struct cmdstruct {
	const char *cmd;
	/* One more member here (fn name) to execute the fun corresponding to the cmd (skipping those details)*/
	unsigned int option; // Won't make use of this member as well
};

static struct cmdstruct commands[] = {
	{ "add", RUN_SETUP | NEED_WORK_TREE },
	{ "push", RUN_SETUP },
	{ "merge", RUN_SETUP | NEED_WORK_TREE },
	{ "blame",  RUN_SETUP },
	{ "cherry-pick", RUN_SETUP | NEED_WORK_TREE },
	{ "clone" },
	{ "fetch", RUN_SETUP },
	{ "commit", RUN_SETUP | NEED_WORK_TREE },
	{ "diff", NO_PARSEOPT },
	{ "revert", RUN_SETUP | NEED_WORK_TREE  },
	{ "init" },
	{ "status", RUN_SETUP | NEED_WORK_TREE },
	{ "pull", RUN_SETUP | NEED_WORK_TREE },
	{ "stash", RUN_SETUP | NEED_WORK_TREE },
	{ "stage", RUN_SETUP | NEED_WORK_TREE },
	{ "rebase",  RUN_SETUP | NEED_WORK_TREE },
	{ "remote", RUN_SETUP },
	{ "status", RUN_SETUP | NEED_WORK_TREE }
};

int main(int argc, char *argv[])
{
	if(argc != 2){
		fprintf(stderr, "Wrong number of arguments passed \n");
		exit(0);
	}
	const char *input_cmd = argv[1];
	printf("Input git command is : %s \n", input_cmd);
	
  help_unknown_cmd(input_cmd);
	return 0;
}

static int cmdname_compare(const void *a_, const void *b_)
{
	struct cmdname *a = *(struct cmdname **)a_;
	struct cmdname *b = *(struct cmdname **)b_;
	return strcmp(a->name, b->name);
}

static int int_compare(const void *a, const void *b)
{
	int *a_int = (int*)a;
	int *b_int = (int*)b;
	return *a_int >= *b_int;
}

static int levenshtein_compare(const void *a_, const void *b_)
{
	struct cmdname *a = *(struct cmdname **)a_;
	struct cmdname *b = *(struct cmdname **)b_;

	int l1 = a->len;
	int l2 = b->len;
	return l1 != l2 ? l1 - l2 : strcmp(a->name, b->name);
}

void help_unknown_cmd(const char *cmd)
{
	int i, n, best_similarity = 0;
	struct cmdnames main_cmds, other_cmds;
	memset(&main_cmds, 0, sizeof(main_cmds));
	memset(&other_cmds, 0, sizeof(other_cmds));
	load_cmd_list("git-", &main_cmds, &other_cmds);
	QSORT(main_cmds.names, main_cmds.cnt, cmdname_compare);
	uniq(&main_cmds);

	for (i = 0, n = 0; i < main_cmds.cnt; i++){
		const char *candidate = main_cmds.names[i]->name;
		main_cmds.names[i]->len = levenshtein(cmd, candidate);
	}
	QSORT(main_cmds.names, main_cmds.cnt, levenshtein_compare);
	/* skip and count prefix matches */
	/* len = 0 when input cmd matches candidate cmd */
	for (n = 0; n < main_cmds.cnt && !main_cmds.names[n]->len; n++)
		;

	/* count all the most similar ones */
	for (best_similarity = main_cmds.names[n++]->len; (n < main_cmds.cnt && best_similarity == main_cmds.names[n]->len); n++)
		;

	fprintf(stderr, ("git: '%s' is not a git command. See 'git --help'."), cmd);
	if (SIMILAR_ENOUGH(best_similarity)) {
		const char *msg = "\nThe most similar command is";
		if (n > 1){
			msg = "\nThe most similar commands are";
		}
		fprintf(stderr, (msg), "");

		printf("\n");
		for (i = 0; i < n; i++)
			fprintf(stderr, "\t%s\n", main_cmds.names[i]->name);
	}

	exit(1);
}

void load_cmd_list(const char *prefix, struct cmdnames *main_cmds, struct cmdnames *other_cmds){
	const char *envPath = getenv("PATH");
	const char *exec_path = git_exec_path();
	load_builtin_commands(prefix, main_cmds);
	// other_cmds are loaded from envPaths, skipping this..
	exclude_cmds(other_cmds, main_cmds);
}

void load_builtin_commands(const char *prefix, struct cmdnames *main_cmds){
	const char *name;
	int i;

	/* Prefix must start with git- else report bug */
	if(!skip_prefix(prefix, "git-", &prefix)){
		printf("Prefix %s must start with 'git-'", prefix);
		exit(0);
	}

	for(i = 0; i < ARRAY_SIZE(commands); i++){
		if(skip_prefix(commands[i].cmd, prefix, &name)){
			add_cmdname(main_cmds, name, strlen(name));
		}
		
	}
}


int skip_prefix(const char *str, const char *prefix, const char **out){
	do{
		if(!*prefix){
			*out = str;
			return 1;
		}
	} while(*str++ == *prefix++);
	return 0;
}

void add_cmdname(struct cmdnames *main_cmds, const char *name, int len){
	struct cmdname *ent;
	FLEX_ALLOC_MEM(ent, name, name, len);
	ent->len = len;

	// Alloc grow on main_cmds
	ALLOC_GROW(main_cmds->names, main_cmds->cnt + 1, main_cmds->alloc);
	main_cmds->names[main_cmds->cnt++] = ent;
}

void uniq(struct cmdnames *cmds)
{
	int i;
	int j = 0;
	if(!cmds->cnt){
		return;
	}
	for(i = 1; i < cmds->cnt; i++){
		if(!strcmp(cmds->names[i]->name, cmds->names[j]->name)){
			free(cmds->names[i]);
		}
		else{
			j += 1;
			cmds->names[j] = cmds->names[i];
		}
	}
	cmds->cnt = j + 1;
}

void exclude_cmds(struct cmdnames *cmds, struct cmdnames *excludes)
{
	int ci, cj, ei;
	int cmp;

	ci = cj = ei = 0;
	while (ci < cmds->cnt && ei < excludes->cnt){
		cmp = strcmp(cmds->names[ci]->name, excludes->names[ei]->name);
		if (cmp < 0){
			cmds->names[cj++] = cmds->names[ci++];
		}
		else if(cmp == 0){
			ei++;
			free(cmds->names[ci++]);
		}
		else {
			ei++;
		}
	}

	while (ci < cmds->cnt){
		cmds->names[cj++] = cmds->names[ci++];
	}
	cmds->cnt = cj;
}

int levenshtein(const char *string1, const char *string2)
{
	// Actual git implementation uses Damerau-Levenshtein (weighted) algo
	int len1 = strlen(string1);
	int len2 = strlen(string2);
	int dp[len1 + 1][len2 + 1];
	for(int i = 0; i <= len2; i++){
		dp[0][i] = i;
	}

	for(int j = 0; j <= len1; j++){
		dp[j][0] = j;
	}
	for(int i = 1; i <= len1; i++){
		for(int j = 1; j <= len2; j++){
			if(string1[i-1] == string2[j-1]){
				dp[i][j] = dp[i-1][j-1];
			}
			else{
				dp[i][j] = 1 + MIN(dp[i-1][j-1], MIN(dp[i][j-1], dp[i-1][j]));
			}
		}
	}

	return dp[len1][len2];
}
