#ifndef NANO_UT_HEAP_H
#define NANO_UT_HEAP_H

typedef
struct looped_heap {
	size_t data_limit;
	size_t loop_size;  // [ .. data_limit]
	size_t begin;      // [0 .. loop_size]
	size_t end;        // [begin .. begin + loop_size]
	char *data;
} looped_heap_t;


/* -------------------------------------------------------------------------- */
int lh_init(looped_heap_t *h, size_t data_limit);
void lh_free(looped_heap_t *h);
void lh_empty(looped_heap_t *h);

int lh_grow(looped_heap_t *h, size_t size);

int lh_get(looped_heap_t *h, size_t size);
int lh_put(looped_heap_t *h, size_t size);

int lh_strdup(looped_heap_t *h, char const *s);
int lh_strfree(looped_heap_t *h, int ofs);

int lh_stat(looped_heap_t *h, size_t *free_size, size_t *free_solid);

#endif
