#include <errno.h>
#include <time.h>

#include "nano/ut.h"
#include "nano/ut_sleep.h"


/* ------------------------------------------------------------------------ */
int ut_usleep(unsigned long us)
{
	struct timespec req = {.tv_sec = us / 1000000, .tv_nsec = (us % 1000000) * 1000};
	struct timespec left;
	while (nanosleep(&req, &left) < 0) {
		if (errno != EINTR)
			return -1;
		req = left;
	}
	return 0;
}

