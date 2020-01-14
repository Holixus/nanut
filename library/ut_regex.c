#define _XOPEN_SOURCE 500

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#include <regex.h>

#include "nano/ut.h"
#include "nano/ut_regex.h"

/* ------------------------------------------------------------------------- */
const char *regex_find_ex(const char *source, const char *regex, ...)
{
	regex_t r;
	if (regcomp(&r, regex, REG_EXTENDED) != 0) {
		syslog(LOG_DEBUG, "regex error: '%s'", regex);
		return NULL;
	}

	va_list ap;
	va_start(ap, regex);

	char const *ret = NULL;
	regmatch_t m[10];
	if (!regexec(&r, source, countof(m), m, 0)) {
		unsigned int i = 1;
		ret = source + m[0].rm_eo;
		for (; i < countof(m); ++i) {
			if (m[i].rm_so < 0)
				break;

			char *dest = va_arg(ap, char*);
			if (!dest)
				break;

			unsigned int size = va_arg(ap, unsigned int);
			size_t len = (size_t) (m[i].rm_eo - m[i].rm_so);
			if (len > size - 1)
				len = size - 1;
			memcpy(dest, source + m[i].rm_so, len);
			dest[len] = 0;
		}
	}

	va_end(ap);
	regfree(&r);
	return ret; /* end of matched expression */
}

/* ------------------------------------------------------------------------- */
const char *regex_find_const_ref(const char *source, const char *regex, ...)
{
	regex_t r;
	if (regcomp(&r, regex, REG_EXTENDED) != 0) {
		syslog(LOG_DEBUG, "regex error: '%s'", regex);
		return NULL;
	}

	va_list ap;
	va_start(ap, regex);

	char const *ret = NULL;
	regmatch_t m[10];
	if (!regexec(&r, source, countof(m), m, 0)) {
		unsigned int i = 1;
		ret = source + m[0].rm_eo;
		for (; i < countof(m); ++i) {
			if (m[i].rm_so < 0)
				break;

			char const **start = va_arg(ap, char const**);
			char const **end = va_arg(ap, char const**);
			if (!start)
				break;

			*start = source + m[i].rm_so;
			if (end)
				*end = source + m[i].rm_eo;
		}
	}

	va_end(ap);
	regfree(&r);
	return ret;
}

/* ------------------------------------------------------------------------- */
char *regex_find_ref(char *source, const char *regex, ...)
{
	regex_t r;
	if (regcomp(&r, regex, REG_EXTENDED) != 0) {
		syslog(LOG_DEBUG, "regex error: '%s'", regex);
		return NULL;
	}

	va_list ap;
	va_start(ap, regex);

	char *ret = NULL;
	regmatch_t m[10];
	if (!regexec(&r, source, countof(m), m, 0)) {
		unsigned int i = 1;
		ret = source + m[0].rm_eo;
		for (; i < countof(m); ++i) {
			if (m[i].rm_so < 0)
				break;

			char **start = va_arg(ap, char **);
			char **end = va_arg(ap, char **);
			if (!start)
				break;

			*start = source + m[i].rm_so;
			if (end)
				*end = source + m[i].rm_eo;
		}
	}

	va_end(ap);
	regfree(&r);
	return ret;
}

/* ------------------------------------------------------------------------- */
int regex_find_all_const_cb(char const *source, const char *regex,
	char const *(*cb)(char const **startp, char const **endp, void *ptr), void *ptr)
{
	regex_t r;
	if (regcomp(&r, regex, REG_EXTENDED) != 0) {
		syslog(LOG_DEBUG, "regex error: '%s'", regex);
		return 0;
	}

	regmatch_t m[10];

	int count = 0;
	for (; !regexec(&r, source, countof(m), m, 0); ++count) {
		const char *startp[countof(m)], *endp[countof(m)];
		int i;
		for (i = 0; i < countof(m); ++i)
			if (m[i].rm_so < 0)
				startp[i] = endp[i] = NULL;
			else {
				startp[i] = source + m[i].rm_so;
				endp[i] = source + m[i].rm_eo;
			}

		if (!(source = cb(startp, endp, ptr)))
			break;
	}

	regfree(&r);
	return count; /* returns number of matches */
}

/* ------------------------------------------------------------------------- */
int regex_find_all_cb(char *source, const char *regex,
	char *(*cb)(char **startp, char **endp, void *ptr), void *ptr)
{
	regex_t r;
	if (regcomp(&r, regex, REG_EXTENDED) != 0) {
		syslog(LOG_DEBUG, "regex error: '%s'", regex);
		return 0;
	}

	regmatch_t m[10];

	int count = 0;
	for (; !regexec(&r, source, countof(m), m, 0); ++count) {
		char *startp[countof(m)], *endp[countof(m)];
		int i;
		for (i = 0; i < countof(m); ++i)
			if (m[i].rm_so < 0)
				startp[i] = endp[i] = NULL;
			else {
				startp[i] = source + m[i].rm_so;
				endp[i] = source + m[i].rm_eo;
			}

		if (!(source = cb(startp, endp, ptr)))
			break;
	}

	regfree(&r);
	return count; /* returns number of matches */
}



/*

static int examples(char login[32], char plan_name[64])
{
	char raw_soap[] = "\
<info>\n
	<login>ololo</login>\n\
	<plan_name>wow-plan!</plan_name>\n\
\</info>\n\
";
	
	char *body;
	if (regex_find_ref(raw_soap, "<info>(.*)</info>", &body, NULL)) {
		*login = *plan_name = 0;
		int fail = !regex_find_ex(body, "<login[^>]*>([^<]*)</login>|<login/>", login, sizeof login);
		fail |= !regex_find_ex(body, "<plan_name[^>]*>(.*)</plan_name>|<plan_name/>", plan_name, sizeof plan_name);
		if (!fail)
			return 0;
	}
	return -1;
}


*/

