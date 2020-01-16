#include "stdio.h"
#include "string.h"


#include "main.h"

#include "nano/ut.h"
#include "nano/ut_str.h"


static int url_gethost_test(char const *url, char const *expected)
{
	char result[128];
	url_gethost(result, sizeof result, url);
	if (strcmp(result, expected)) {
		printf("    [FAILED] \"%s\" -> \"%s\" but expected \"%s\"\n", url, result, expected);
		return 0;
	}
	if (PRINT_OK_MESSAGES)
		printf("    [OK] \"%s\" -> \"%s\"\n", url, result);
	return 1;
}


static char const * const url_host_samples[] = {
	"https://1.1.1.1/path",    "1.1.1.1",
	"https://1.1.1.1",         "1.1.1.1",
	"http://acs.rt.ru",        "acs.rt.ru",
	"http://acs.rt.ru?df",     "acs.rt.ru",
	"http://acs.rt.ru#dfdf",   "acs.rt.ru",
	"http://acs.rt.ru:8080#aa","acs.rt.ru"
};

int url_tests()
{
	printf("Test url_gethost()\n");
	char const * const *s = url_host_samples;
	char const * const *se = url_host_samples + countof(url_host_samples);
	while (s < se) {
		if (!url_gethost_test(s[0], s[1]))
			return 0;
		s += 2;
	}

	return 1;
}
