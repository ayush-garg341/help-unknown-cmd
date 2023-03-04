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
	{ "blame",  RUN_SETUP },
	{ "cherry-pick", RUN_SETUP | NEED_WORK_TREE },
	{ "clone" },
	{ "commit", RUN_SETUP | NEED_WORK_TREE },
	{ "diff", NO_PARSEOPT },
	{ "fetch", RUN_SETUP },
	{ "init" },
	{ "merge", RUN_SETUP | NEED_WORK_TREE },
	{ "pull", RUN_SETUP | NEED_WORK_TREE },
	{ "push", RUN_SETUP },
	{ "rebase",  RUN_SETUP | NEED_WORK_TREE },
	{ "remote", RUN_SETUP },
	{ "revert", RUN_SETUP | NEED_WORK_TREE  },
	{ "stash", RUN_SETUP | NEED_WORK_TREE },
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
	
	const char *help = help_unknown_cmd(input_cmd);
	printf("%s\n", help);
	return 0;
}

const char *help_unknown_cmd(const char *cmd)
{
	int i, n, best_similarity = 0;
	struct cmdnames main_cmds, other_cmds;
	memset(&main_cmds, 0, sizeof(main_cmds));
	memset(&other_cmds, 0, sizeof(other_cmds));
	load_cmd_list("git-", &main_cmds, &other_cmds);

	return "Implementation going on";
}

void load_cmd_list(const char *prefix, struct cmdnames *main_cmds, struct cmdnames *other_cmds){
	const char *envPath = getenv("PATH");
	const char *exec_path = git_exec_path();
	printf("PATH : %s\n ", envPath);
	printf("EXEC PATH : %s\n", exec_path);
	load_builtin_commands(prefix, main_cmds);
}

void load_builtin_commands(const char *prefix, struct cmdnames *main_cmds){
	const char *name;
	int i;

	printf("Before skipping, prefix is %s\n", prefix);
	/* Prefix must start with git- else report bug */
	if(!skip_prefix(prefix, "git-", &prefix)){
		printf("Prefix %s must start with 'git-'", prefix);
		exit(0);
	}

	printf("After skipping, prefix is %s\n", prefix);
	for(i = 0; i < ARRAY_SIZE(commands); i++){
		if(skip_prefix(commands[i].cmd, prefix, &name)){
			printf("Command is %s\n", name);
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
}
