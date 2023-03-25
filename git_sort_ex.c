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

const char *git_commands[13] = {
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
    "add",
    "stash",
    "remote"
};

int main(int argc, char *argv[])
{
    struct cmdnames main_cmds;
	  memset(&main_cmds, 0, sizeof(main_cmds));
    load_cmd_list(&main_cmds);
    printf("===============Before sorting==============\n");
    for(int i = 0; i < main_cmds.cnt; i++){
        printf("%s ", main_cmds.names[i]->name);
    }
    printf("\n");
    QSORT(main_cmds.names, main_cmds.cnt, cmdname_compare);
    printf("==============After sorting================\n");
    for(int i = 0; i < main_cmds.cnt; i++){
        printf("%s ", main_cmds.names[i]->name);
    }
    printf("\n");
};

void load_cmd_list(struct cmdnames *main_cmds){
    ALLOC_GROW(main_cmds->names, 20);
    for(int i = 0; i < 13; i++){
        const char *name = git_commands[i];
        size_t len = strlen(name);
        struct cmdname *ent;
        ent = calloc(1, sizeof(*(ent))+len+1);
        memcpy(ent->name, name, len);
        ent->len = len;
        main_cmds->names[main_cmds->cnt++] = ent;
    }
}


void msort_with_tmp(void *b, size_t n, size_t s,
		            int (*cmp)(const void *, const void *),
								char *t)
{
	char *tmp;
	char *b1, *b2;
	size_t n1, n2;

	if(n <= 1){
		return;
	}
	n1 = n/2;
	n2 = n - n1;
	b1 = b;
	b2 = (char *)b + (n1 * s);
	msort_with_tmp(b1, n1, s, cmp, t);
	msort_with_tmp(b2, n2, s, cmp, t);

	tmp = t;
	while(n1 > 0 && n2 > 0){
		if(cmp(b1, b2) <= 0){
			memcpy(tmp, b1, s);
			tmp += s;
			b1 += s;
			--n1;
		}
		else{
			memcpy(tmp, b2, s);
			tmp += s;
			b2 += s;
			--n2;
		}
	}
	if(n1 > 0)
		memcpy(tmp, b1, n1 * s); // if after comparing two sorted sub arrays, we are left with elements in left sub-arrays, just copy those.
  if(n2 > 0)
    memcpy(tmp, b2, n2*s);
	memcpy(b, t, (n1+n2)*s); // if after comparing two sorted sub arrays, we are left with elements in right sub-arrays, just copy those.

  // But it will never happen that at the end elements are present in both sub-arrays.
}

void git_stable_qsort(void *b, size_t n, size_t s, int* cmp(const void *, const void *))
{
	const size_t size = n * s;
	char *tmp;
	tmp = malloc(size);
	msort_with_tmp(b, n, s, cmp, tmp);
	free(tmp);
}
