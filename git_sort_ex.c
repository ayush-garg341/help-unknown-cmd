#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define ALLOC_GROW(x, alloc) (x) = realloc((x), sizeof(*(x))*(alloc))

struct cmdnames{
    int cnt;
    struct cmdname{
        size_t len;
        char name[1];
    } **names;
};

static int cmdname_compare(const void *a_, const void *b_)
{
	struct cmdname *a = *(struct cmdname **)a_;
	struct cmdname *b = *(struct cmdname **)b_;
	return strcmp(a->name, b->name);
}

static void sane_qsort(void *base, size_t nmemb, size_t size,
									int *compare(const void *, const void *))
{
	if(nmemb > 1){
		qsort(base, nmemb, size, compare);
	}
}

void git_stable_qsort(void *base, size_t nmemb, size_t size, int *compare(const void *, const void *));

#define qsort git_stable_qsort

#define QSORT(base, n, compare) sane_qsort((base), (n), sizeof(*base), compare)

void load_cmd_list(struct cmdnames *main_cmds);

const char *git_commands[12] = {
    "add",
    "push",
    "merge",
    "stage",
    "blame",
    "clone",
    "cherry-pick",
    "rebase",
    "fetch",
    "commit",
    "stash",
    "remote"
};

int main(int argc, char *argv[])
{
    struct cmdnames main_cmds;
    struct cmdname ent;
    printf("size of main cmds : %ld\n", sizeof(main_cmds));
	  memset(&main_cmds, 0, sizeof(main_cmds));
    printf("size of cmd name : %ld\n", sizeof(ent));
    load_cmd_list(&main_cmds);
	  QSORT(main_cmds.names, main_cmds.cnt, cmdname_compare);
};

void load_cmd_list(struct cmdnames *main_cmds){
    ALLOC_GROW(main_cmds->names, 20);
    for(int i = 0; i < 12; i++){
        const char *name = git_commands[i];
        size_t len = strlen(name);
        struct cmdname *ent;
        ent = calloc(1, sizeof(*(ent))+len+1);
        memcpy(ent->name, name, len);
        main_cmds->names[main_cmds->cnt++] = ent;
    }
    printf("%d\n", main_cmds->cnt);
}
