#ifndef NANO_UT_MEM_H
#define NANO_UT_MEM_H

#ifdef ENABLE_MEMORY_STATISTICS

#define CONFIG_MEM_CONTEXTS_LIMIT 20

/* -------------------------------------------------------------------------- */
typedef
struct {
	char id[16];
	int size;
	int peak;
	int allocs_counter;
	int frees_counter;
} ut_mem_stat_t;


/* -------------------------------------------------------------------------- */
extern ut_mem_stat_t ut_mem_stat_total;
extern ut_mem_stat_t ut_mem_stat_other;

extern ut_mem_stat_t ut_mem_stat_list[CONFIG_MEM_CONTEXTS_LIMIT];
extern int ut_mem_stat_count;

extern ut_mem_stat_t *ut_mem_stat_context;


/* -------------------------------------------------------------------------- */
void ut_mem_set_context(char const *id);

enum ut_mem_stat_op { IMS_ALLOC, IMS_FREE };

void ut_mem_stat(ut_mem_stat_t *ctx, int op, size_t size);


void *ut_mem_malloc(char const *ctx, size_t size);
void *ut_mem_calloc(char const *ctx, size_t nmemb, size_t size);
void ut_mem_free(char const *ctx, void *p);
void *ut_mem_realloc(char const *ctx, void *p, size_t size);
void *ut_mem_reallocarray(char const *ctx, void *p, size_t nmemb, size_t size);



#else

#define ut_mem_set_context(id)
#define ut_mem_malloc(ctx, size)                     malloc(size)
#define ut_mem_calloc(ctx, nmemb, size)              calloc(nmemb, size)
#define ut_mem_free(ctx, ptr)                        free(ptr)
#define ut_mem_realloc(ctx, ptr, size)               realloc(ptr, size)
#define ut_mem_reallocarray(ctx, ptr, nmemb, size)   reallocarray(ptr, nmemb, size)

#endif

#endif
