#ifndef NANUT_REGEX_H
#define NANUT_REGEX_H

const char *regex_find_ex(const char *source, const char *regex, ...);  /* char *buf1, size_t len1, ... */
const char *regex_find_const_ref(const char *source, const char *regex, ...); /* char **start1, char **end1, ... */
char *regex_find_ref(char *source, const char *regex, ...); /* char **start1, char **end1, ... */

int regex_find_all_const_cb(char const *source, const char *regex,
	char const *(*callback)(char const **startp, char const **endp, void *ptr), void *ptr);

int regex_find_all_cb(char *source, const char *regex,
	char *(*callback)(char **startp, char **endp, void *ptr), void *ptr);


#endif
