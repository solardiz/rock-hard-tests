#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/wait.h>

#include "glibc-all.h"

static const struct {
	char *test_name;
	int (*test_func)(void);
} tests[] = {
	{"asprintf(3) should set pointer to NULL on error", glibc_asprintf},
	{"assert(3) should be unaffected by CVE-2025-0395", glibc_assert},
	{"fread(3)/fwrite(3) should detect integer overflow", glibc_freadwrite},
	{"qsort(3) robustness with nontransitive comparison", glibc_qsort}
};

int main(void)
{
	int fdi, fdo;
	if ((fdi = open("/dev/zero", O_RDONLY)) == -1 ||
	    (fdo = open("/dev/null", O_WRONLY)) == -1) {
		perror("open");
		return 1;
	}

	for (unsigned int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
		printf("Test #%u: %s ...\t", i + 1, tests[i].test_name);
		fflush(stdout);

		test_result result = TEST_SKIP_FAIL;
		int status;
		switch (fork()) {
		case -1:
			perror("fork");
			break;
		case 0:
			if (dup2(fdi, 0) == -1 || dup2(fdo, 1) == -1)
				perror("dup2");
			return tests[i].test_func();
		default:
			if (wait(&status) == -1)
				perror("wait");
			else
				result = WEXITSTATUS(status);
		}

		switch (result) {
		case TEST_PASS:
			puts("PASSED");
			break;
		case TEST_FAIL:
			puts("FAILED");
			break;
		case TEST_SKIP_FAIL:
			puts("SKIPPED - prerequisite setup FAILED");
			break;
		case TEST_SKIP_NA:
			puts("SKIPPED - not applicable");
			break;
		default:
			printf("UNKNOWN(%d) - assume FAILED\n", result);
		}
	}

	return 0;
}
