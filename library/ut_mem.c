#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "nano/ut.h"
#include "nano/ut_mem.h"

#ifdef ENABLE_MEMORY_STATISTICS

#define CONFIG_MEM_CONTEXTS_LIMIT 20


/* -------------------------------------------------------------------------- */
ut_mem_stat_t ut_mem_stat_total = { .id = "total" };
ut_mem_stat_t ut_mem_stat_other = { .id = "other" };

ut_mem_stat_t ut_mem_stat_list[CONFIG_MEM_CONTEXTS_LIMIT];
int ut_mem_stat_count;

ut_mem_stat_t *ut_mem_stat_context;


/* -------------------------------------------------------------------------- */
void ut_mem_set_context(char const *id)
{
	ut_mem_stat_t *p = ut_mem_stat_list, *e = ut_mem_stat_list + ut_mem_stat_count;
	for (; p < e; ++p)
		if (!strncmp(p->id, id, sizeof ut_mem_stat_context->id - 1)) {
			ut_mem_stat_context = p;
			return;
		}

	if (ut_mem_stat_count >= CONFIG_MEM_CONTEXTS_LIMIT) {
		ut_mem_stat_context = &ut_mem_stat_other;
		static int c = 0;
		if (!c)
			syslog(LOG_ERR, "ut_mem_set_context(\"%s\"): need more mem contexts (now is %d)", id, c = CONFIG_MEM_CONTEXTS_LIMIT);
		return ;
	}

	ut_mem_stat_context = ut_mem_stat_list + ut_mem_stat_count++;
	strncpy(ut_mem_stat_context->id, id, sizeof ut_mem_stat_context->id - 1);
	ut_mem_stat_context->id[sizeof ut_mem_stat_context->id - 1] = 0;
}

/* -------------------------------------------------------------------------- */
void ut_mem_stat(ut_mem_stat_t *ctx, int op, size_t size)
{
	switch (op) {
	case IMS_ALLOC:
		if (size) {
			ctx->size += size;
			++ctx->allocs_counter;
			if (ctx->size > ctx->peak)
				ctx->peak = ctx->size;
		}
		break;
	case IMS_FREE:
		if (size) {
			ctx->size -= size;
			++ctx->frees_counter;
		}
		break;
	}
}


/* -------------------------------------------------------------------------- */
void *ut_malloc(char const *ctx, size_t size)
{
	ut_mem_set_context(ctx);
	ut_mem_stat(&ut_mem_stat_total,  IMS_ALLOC, size);
	ut_mem_stat(ut_mem_stat_context, IMS_ALLOC, size);
	size_t *p = malloc(size + sizeof size);
	*p = size;
	return p + 1;
}

/* -------------------------------------------------------------------------- */
void *ut_calloc(char const *ctx, size_t nmemb, size_t size)
{
	ut_mem_set_context(ctx);
	ut_mem_stat(&ut_mem_stat_total,  IMS_ALLOC, size);
	ut_mem_stat(ut_mem_stat_context, IMS_ALLOC, size);
	size_t *p = calloc(nmemb, size + sizeof size);
	*p = size;
	return p + 1;
}

/* -------------------------------------------------------------------------- */
void ut_free(char const *ctx, void *p)
{
	ut_mem_set_context(ctx);
	size_t *f = (size_t *)p - 1;
	size_t size = *f;
	ut_mem_stat(&ut_mem_stat_total,  IMS_FREE, size);
	ut_mem_stat(ut_mem_stat_context, IMS_FREE, size);
	return free(f);
}

/* -------------------------------------------------------------------------- */
void *ut_realloc(char const *ctx, void *p, size_t size)
{
	ut_mem_set_context(ctx);
	size_t *f = (size_t *)p - 1;
	size_t old_size = *f;
	ut_mem_stat(&ut_mem_stat_total,  IMS_FREE, old_size);
	ut_mem_stat(ut_mem_stat_context, IMS_FREE, old_size);
	ut_mem_stat(&ut_mem_stat_total,  IMS_ALLOC, size);
	ut_mem_stat(ut_mem_stat_context, IMS_ALLOC, size);
	f = realloc(f, size + sizeof old_size);
	*f = size;
	return f + 1;
}

/* -------------------------------------------------------------------------- */
void *ut_reallocarray(char const *ctx, void *p, size_t nmemb, size_t size)
{
	ut_mem_set_context(ctx);
	size_t *f = (size_t *)p - 1;
	size_t old_size = *f;
	ut_mem_stat(&ut_mem_stat_total,  IMS_FREE, old_size);
	ut_mem_stat(ut_mem_stat_context, IMS_FREE, old_size);
	size *= nmemb;
	ut_mem_stat(&ut_mem_stat_total,  IMS_ALLOC, size);
	ut_mem_stat(ut_mem_stat_context, IMS_ALLOC, size);
	f = realloc(f, size + sizeof size);
	*f = size;
	return f + 1;
}

#endif
