#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include "glibc-all.h"

static const struct {
	char *test_name;
	int (*test_func)(void);
} tests[] = {
	{"asprintf", glibc_asprintf}
};

int main(void)
{
	for (unsigned int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
		printf("Test #%u: %s ...\t", i + 1, tests[i].test_name);
		fflush(stdout);

		test_result result = TEST_SKIP_FAIL;
		switch (fork()) {
		case -1:
			perror("fork");
			break;
		case 0:
			return tests[i].test_func();
		default:
			int status;
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
