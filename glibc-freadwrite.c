#include <stdio.h>

#include "glibc-all.h"

int glibc_freadwrite(void)
{
	const size_t f1 = (size_t)1 << (sizeof(size_t) * 2);
	const size_t f2 = ((size_t)1 << (sizeof(size_t) * 7)) + 1;
	char buf[f1];

	if (fread(buf, f1, f2, stdin))
		return TEST_FAIL;

	if (fwrite(buf, f1, f2, stdout))
		return TEST_FAIL;

	return TEST_PASS;
}
