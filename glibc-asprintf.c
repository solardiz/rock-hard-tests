#define _GNU_SOURCE
#include <stdio.h>
#include <sys/resource.h>

#include "glibc-all.h"

int glibc_asprintf(void)
{
	const struct rlimit rlim = {};
	if (setrlimit(RLIMIT_AS, &rlim))
		return TEST_SKIP_FAIL;

	char *p = "a";
	for (int i = 0; i < 30; i++)
		if (asprintf(&p, "%s%s", p, p) == -1)
			return p ? TEST_FAIL : TEST_PASS;

	return TEST_SKIP_FAIL;
}
