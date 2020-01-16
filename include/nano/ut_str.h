#ifndef NANUT_STR_H
#define NANUT_STR_H

char const *strf(char const *format, ...) __attribute__ ((format (printf, 1, 2)));

int str_rechrs(char *str, char ch, char rep);

int url_gethost(char *host, size_t size, char const *url);
int url_getport(char const *url);
char const *url_getpath(char const *url);

#endif
