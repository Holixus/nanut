#include "stdio.h"
#include "string.h"


#include "main.h"

#include "nano/ut.h"
#include "nano/ut_str.h"


/* ------------------------------------------------------------------------ */
static str_str_t const url_gethost_samples[] = {
	{ "https://1.1.1.1/path",    "1.1.1.1" },
	{ "https://1.1.1.1",         "1.1.1.1" },
	{ "http://acs.rt.ru",        "acs.rt.ru" },
	{ "http://acs.rt.ru?df",     "acs.rt.ru" },
	{ "http://acs.rt.ru#dfdf",   "acs.rt.ru" },
	{ "http://acs.rt.ru:8080#aa","acs.rt.ru" }
};


static int url_gethost_test(char const *url, char const *expected)
{
	char result[128];
	url_gethost(result, sizeof result, url);
	if (strcmp(result, expected))
		FAILED("\"%s\" -> \"%s\" but expected \"%s\"\n", url, result, expected);

	SUCCESS("\"%s\" -> \"%s\"\n", url, result);
}


/* ------------------------------------------------------------------------ */
static str_int_t const url_getport_samples[] = {
	{ "https://1.1.1.1/path",    443 },
	{ "https://1.1.1.1",         443 },
	{ "http://acs.rt.ru",        80 },
	{ "http://acs.rt.ru?df",     80 },
	{ "http://acs.rt.ru#dfdf",   80 },
	{ "http://acs.rt.ru:8018#aa",8018 },
	{ "http://rt.ru:123",        123 },
	{ "http://rt.ru:124?df",     124 },
	{ "http://rt.ru:125#dfdf",   125 },
	{ "http://rt.ru:808#aa",     808 },
	{ "ftp://acs.rt.ru",         21 }
};


static int url_getport_test(char const *url, int expected)
{
	int result = url_getport(url);
	if (result != expected)
		FAILED("\"%s\" -> %d but expected %d\n", url, result, expected);

	SUCCESS("\"%s\" -> %d\n", url, result);
}


/* ------------------------------------------------------------------------ */
static str_str_t const url_getpath_samples[] = {
	{ "https://1.1.1.1/path/a",  "/path/a" },
	{ "https://1.1.1.1",         "" },
	{ "http://acs.rt.ru",        "" },
	{ "http://acs.rt.ru?df",     "?df" },
	{ "http://acs.rt.ru#dfdf",   "#dfdf" },
	{ "http://acs.rt.ru:8018#aa","#aa" },
	{ "http://rt.ru:123",        "" },
	{ "http://rt.ru:124?df",     "?df" },
	{ "http://rt.ru:125#dfdf",   "#dfdf" },
	{ "http://rt.ru:808#aa",     "#aa" },
	{ "ftp://acs.rt.ru",         "" }
};


static int url_getpath_test(char const *url, char const *expected)
{
	char const *result = url_getpath(url);
	if (strcmp(result, expected))
		FAILED("\"%s\" -> \"%s\" but expected \"%s\"\n", url, result, expected);

	SUCCESS("\"%s\" -> \"%s\"\n", url, result);
}


/* ------------------------------------------------------------------------ */
int url_tests()
{
	TEST_PAIRS(url_gethost);
	TEST_PAIRS(url_getport);
	TEST_PAIRS(url_getpath);

	return 1;
}
