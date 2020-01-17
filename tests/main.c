#include "stdio.h"

#include "main.h"

/* ------------------------------------------------------------------------ */
int main(int argc, char *argv[])
{
	if (!url_tests())
		return 1;

	if (!str_tests())
		return 1;

	return 0;
}
