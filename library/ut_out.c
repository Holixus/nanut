#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "nano/ut.h"
#include "nano/ut_out.h"

/* ------------------------------------------------------------------------ */
void owrite(out_t *o, char const *str, size_t size)
{
	if (size >= o->e - o->p - 1)
		size = o->e - o->p - 1;
	memcpy(o->p, str, size);
	o->p += size;
}

/* ------------------------------------------------------------------------ */
void oprintf(out_t *o, char const *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	o->p += (size_t)vsnprintf(o->p, (size_t)(o->e - o->p), fmt, ap);
	va_end(ap);
}

/* ------------------------------------------------------------------------ */
void voprintf(out_t *o, char const *fmt, va_list ap)
{
	o->p += (size_t)vsnprintf(o->p, (size_t)(o->e - o->p), fmt, ap);
}


/* ------------------------------------------------------------------------ */
void oend(out_t *o, char delim)
{
	if (o->e <= o->p || o->p[-1] != delim)
		return;
	*--(o->p) = 0;
}

