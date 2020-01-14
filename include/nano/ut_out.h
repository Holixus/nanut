#ifndef NANO_UT_OUT_H
#define NANO_UT_OUT_H

typedef
struct _out {
	char *p, *e;
} out_t;

#define DECL_OUT_T(name, buf)  out_t name = { .p = buf, .e = buf + sizeof buf }

#include <stdarg.h>

void owrite(out_t *o, char const *str, size_t size);

void voprintf(out_t *o, char const *fmt, va_list ap);

void oprintf(out_t *o, char const *fmt, ...) __attribute__ ((format (printf, 2, 3)));

void oend(out_t *o, char delim);

static inline size_t olen(out_t *o, char *begin) {
	return (size_t)(o->p - begin);
}


#endif
