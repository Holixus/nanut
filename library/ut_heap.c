#include <stdlib.h>
#include <string.h>

#include "nano/ut.h"
#include "nano/ut_mem.h"
#include "nano/ut_heap.h"

/* -------------------------------------------------------------------------- */
int lh_init(looped_heap_t *h, size_t data_limit)
{
	h->data = ut_mem_malloc("lh-heap", data_limit);
	if (!h->data)
		return -1;
	h->data_limit = h->loop_size = data_limit;
	h->begin = h->end = 0;
	return 0;
}


/* -------------------------------------------------------------------------- */
void lh_free(looped_heap_t *h)
{
	ut_mem_free("lh-heap", h->data);
	memset(h, 0, sizeof *h);
}


/* -------------------------------------------------------------------------- */
void lh_empty(looped_heap_t *h)
{
	h->begin = h->end = 0;
	h->loop_size = h->data_limit;
}


/* -------------------------------------------------------------------------- */
int lh_grow(looped_heap_t *h, size_t size)
{
	size_t data_limit = h->data_limit + size;
	char *data = ut_mem_malloc("lh-heap", data_limit);
	if (!data)
		return -1;
	if (h->end > h->begin) {
		if (h->end < h->loop_size) {
			memcpy(data, h->data + h->begin, (unsigned)(h->end - h->begin));
		} else {
			memcpy(data, h->data + h->begin, (unsigned)(h->loop_size - h->begin));
			memcpy(data + h->loop_size, h->data, (unsigned)(h->end - h->loop_size));
		}
		h->end -= h->begin;
		h->begin = 0;
	}
	ut_mem_free("lh-heap", h->data);
	h->data = data;
	h->data_limit = h->loop_size = data_limit;
	return 0;
}


/* -------------------------------------------------------------------------- */
int lh_get(looped_heap_t *h, size_t size)
{
	if (h->end < h->loop_size) {
		if (h->loop_size - h->end >= size)
_alloc:
			return ((h->end += size) - size) % h->loop_size;
		h->loop_size = h->end;
		if (size <= h->begin)
			goto _alloc;
		return -1;
	}

	if (h->begin - (h->end - h->loop_size) >= size)
		goto _alloc;

	return -1;
}


/* -------------------------------------------------------------------------- */
int lh_put(looped_heap_t *h, size_t size)
{
	if (h->end - h->begin < size)
		return -1;

	h->begin += size;

	if (h->begin == h->end) {
		h->begin = h->end = 0;
		h->loop_size = h->data_limit;
		return 0;
	}

	if (h->begin >= h->loop_size) {
		h->begin -= h->loop_size;
		h->end   -= h->loop_size;
		h->loop_size = h->data_limit;
	}

	return 0;
}


/* -------------------------------------------------------------------------- */
int lh_strdup(looped_heap_t *h, char const *s)
{
	int ofs = lh_get(h, strlen(s) + 1);
	return ofs >= 0 ? (strcpy(h->data + ofs, s), ofs) : -1;
}


/* -------------------------------------------------------------------------- */
int lh_strfree(looped_heap_t *h, int ofs)
{
	return lh_put(h, strlen(h->data + ofs) + 1);
}


/* -------------------------------------------------------------------------- */
int lh_stat(looped_heap_t *h, size_t *free_size, size_t *free_solid)
{
	if (free_size)
		*free_size = (unsigned)(h->end <= h->loop_size ? h->data_limit - h->end + h->begin : h->begin - h->end);
	if (free_solid) {
		*free_solid = (unsigned)(h->end <= h->loop_size ?
			(h->data_limit - h->end > h->begin ? h->data_limit - h->end : h->begin) :
			h->begin - h->end);
	}
	return 0;
}
