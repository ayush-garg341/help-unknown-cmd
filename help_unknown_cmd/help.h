#ifndef HELP_H
#define HELP_H

#ifndef FLEX_ARRAY
#define FLEX_ARRAY 1
#endif

#include<stdlib.h>

/*
 * These functions help you allocate structs with flex arrays, and copy
 * the data directly into the array. For example if you had
 *     struct foo {
 *        int bar;
 *        char name[FLEX_ARRAY];
 *     };
 *
 *  You can do;
 *
 *  struct foo *f;
 *  FLEX_ALLOC_MEM(foo, name, src, len);
 *
 * to allocate a "foo" with the contents of "src" in the "name" field.
 * The resulting struct is atumatically zero'd and the flex array field
 * is NULL terminated ( whether the incoming src buffer was or not ).
 */
#define FLEX_ALLOC_MEM(x, filename, buf, len) do { \
	size_t flex_array_len_ = (len); \
	(x) = calloc(1, st_add3(sizeof(*(x)), flex_array_len_, 1)); \
	memcpy((void *)(x)->filename, (buf), flex_array_len_); \
} while(0)

/*
 * void *xmalloc(size_t size);
 * void *xcalloc(size_t nmemb, size_t size);
 * void *xrealloc(void *ptr, size_t size);
 * extern void (*xmalloc_error_handler)(const char *, size_t, const char *, int);
 *
 * These functions are equivalent to the normal C library functions malloc(), calloc(), and realloc(),
 * except that the memory allocation is checked and is guaranteed to never return NULL (indicating an out of memory condition)
 * Instead, if the underlying memory allocation fails, the xmalloc_error_handler callback is called.
 *
 * xcalloc() takes a count and an item size and returns enough memory to hold nmemb contiguous objects of size size (in other words, nmemb * size).
 * The memory allocated by xcalloc() will be set to the all-0 bit pattern before it is returned.
 * xmalloc(), xcalloc(), and xrealloc() all return a pointer to the newly allocated or resized memory.
 * All three functions are guaranteed to never return NULL, so their return value does not need to be checked.
 *
 * Copies "numBytes" bytes from address "from" to address "to"
 * void * memcpy(void *to, const void *from, size_t numBytes);
 */

/*
 * Static functions in C are functions that are restricted to the same file in which they are defined.
 * The functions in C are by default global. If we want to limit the scope of the function, we use the keyword static before the function
 * Inline -> function whose definitions are small and be substituted at the place where its function call is happened.
 * Function substitution is totally compiler choice.
 */
static inline size_t st_add(size_t a, size_t b)
{
	return a + b;
}
#define st_add3(a,b,c)   st_add(st_add((a),(b)),(c))


struct cmdnames {
	int alloc;
	int cnt;
	struct cmdname {
		size_t len;
		// Flexible Array Member(FAM)
		// variable length array must be last member of structure
		char name[FLEX_ARRAY];
	} **names;
};

const char *help_unknown_cmd(const char *cmd);
void load_cmd_list(const char *prefix, struct cmdnames *main_cmds, struct cmdnames *other_cmds);
void load_builtin_commands(const char *prefix, struct cmdnames *main_cmds);
int skip_prefix(const char *str, const char *prefix, const char **out);
void add_cmdname(struct cmdnames *main_cmds, const char *name, int len);

#endif /* HELP_H */
