#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <syslog.h>

#include <ctype.h>
#include <string.h>

#include <sys/types.h>

#include "nano/ut.h"
#include "nano/ut_str.h"

/* ------------------------------------------------------------------------ */
char const *strf(char const *format, ...)
{
	static char buf[8][256];
	static int index = 0;
	char *str = buf[index++ & 7];
	va_list ap;
	va_start(ap, format);
	vsnprintf(str, sizeof buf[0], format, ap);
	va_end(ap);
	return str;
}


/* ------------------------------------------------------------------------ */
int url_getport(char const *url)
{
	char const *h = strchr(url, ':'); // skip service name
	if (!h || h[1] != '/' || h[2] != '/') {
		//syslog(LOG_DEBUG, "bad url '%s'", url);
		return -1;
	}

	h += 3;
	char const *e = strchr(h, '@'); // skip login:pass@ part
	e = e ? e + 1 : h;
	for (; *e && *e != ':' && *e != '/' && *e != '?' && *e != '#'; ++e)
		; // skip host

	int port = 0;
	if (*e == ':') {
		port = atoi(e + 1);
		goto _ret;
	}

	if (!memcmp(url, "http", 4)) {
		if (url[4] == 's' && url[5] == ':')
			port = 443;
		else
			if (url[4] == ':')
				port = 80;
	} else {
		if (url[0] == 'f' && url[1] == 't' && url[2] == 'p' && url[3] == ':')
			port = 21;
	}
_ret:
	//syslog(LOG_DEBUG, "'%s' -> port %d", url, port);
	return port;
}


/* ------------------------------------------------------------------------ */
int str_rechrs(char *str, char ch, char rep)
{
	while ((str = strchr(str, ch)))
		*str++ = rep;
	return 0;
}


/* ------------------------------------------------------------------------ */
char const *url_getpath(char const *url)
{
	char const *h = strchr(url, ':'); // skip service name
	if (!h || h[1] != '/' || h[2] != '/') {
		//syslog(LOG_DEBUG, "bad url '%s'", url);
		return NULL;
	}

	h += 3;
	char const *e = strchr(h, '@'); // skip login:pass@ part
	e = e ? e + 1 : h;
	for (; *e && *e != '/' && *e != '?' && *e != '#'; ++e)
		; // skip host:port

	return e;
}

