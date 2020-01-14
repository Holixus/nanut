#ifndef NANUT_STR_H
#define NANUT_STR_H

typedef
struct _out {
	char *p, *e;
} out_t;

void oprintf(out_t *o, char const *format, ...) __attribute__ ((format (printf, 2, 3)));
void oend(out_t *o, char delim);

char const *strf(char const *format, ...) __attribute__ ((format (printf, 1, 2)));

int url_getport(char const *url);
char const *url_getpath(char const *url);

#endif
