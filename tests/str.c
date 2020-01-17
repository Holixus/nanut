
#include "stdio.h"
#include "string.h"


#include "main.h"

#include "nano/ut.h"
#include "nano/ut_str.h"


typedef struct {
	char const *str;
	char a;
	char b;
} str_cc_t;


/* ------------------------------------------------------------------------ */
static int str_replace_char_test(str_cc_t const *sample, char const *expected)
{
	char result[256];
	snprintf(result, sizeof result, "%s", sample->str);
	str_replace_char(result, sample->a, sample->b);

	if (strcmp(result, expected))
		FAILED("\"%s\", '%c', '%c' -> \"%s\" but expected \"%s\"\n", sample->str, sample->a, sample->b, result, expected);

	SUCCESS("\"%s\", '%c', '%c' -> \"%s\"\n", sample->str, sample->a, sample->b, result);
}

struct {
	str_cc_t sample;
	char const *result;
} const  str_replace_char_samples[] = {
	{ { "asdaerg3412 1345t ", 'a', '-' }, "-sd-erg3412 1345t " },
	{ { "asdaerg3412 1345t ", '-', '-' }, "asdaerg3412 1345t " },
	{ { "", '-', '-' }, "" }
};

/* ------------------------------------------------------------------------ */
int str_tests()
{
	TEST_STRUCT_PAIRS(str_replace_char);
	return 1;
}
