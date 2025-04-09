/* glibc quick sort test originally by Qualys, enhanced for Rocky Linux */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

#include "glibc-all.h"

static int cmp(const void * const pa, const void * const pb)
{
	const int a = *(const int *)pa;
	const int b = *(const int *)pb;
	return a - b;
}

int glibc_qsort(void)
{
	struct rlimit rlim_orig;
	if (getrlimit(RLIMIT_AS, &rlim_orig))
		return TEST_SKIP_FAIL;

	for (size_t nmemb = 12345; nmemb < (1 << 22); nmemb += 131071) {
		int * const pcanary1 = calloc(1 + nmemb + 1, sizeof(int));
		if (!pcanary1)
			return TEST_SKIP_FAIL;
		int * const array = pcanary1 + 1;
		int * const pcanary2 = array + nmemb;

		const int canary1 = *pcanary1 = -random();
		const int canary2 = *pcanary2 = -random();
		array[random() % nmemb] = INT_MIN;

		/* Force fallback from merge to quick sort */
		const struct rlimit rlim_zero = {0, rlim_orig.rlim_max};
		if (setrlimit(RLIMIT_AS, &rlim_zero))
			return TEST_SKIP_FAIL;

		qsort(array, nmemb, sizeof(int), cmp);

		for (size_t i = 0; i < nmemb; i++)
			array[i] = random();

		qsort(array, nmemb, sizeof(int), cmp);

		if (*pcanary1 != canary1)
			return TEST_FAIL;
		if (*pcanary2 != canary2)
			return TEST_FAIL;

		for (size_t i = 1; i < nmemb; i++)
			if (array[i - 1] > array[i])
				return TEST_FAIL;

		if (setrlimit(RLIMIT_AS, &rlim_orig))
			return TEST_SKIP_FAIL;

		free(pcanary1);
	}

	return TEST_PASS;
}
