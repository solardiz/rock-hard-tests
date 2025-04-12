#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/user.h>
#include <sys/wait.h>

extern char *__progname;

#include "glibc-all.h"

#define MSG_SUFFIX ": Assertion `0' failed.\n"

static ssize_t read_loop(int fd, void *buffer, size_t count)
{
	ssize_t offset, block;

	errno = 0;

	offset = 0;
	while (count > 0 /* && count <= SSIZE_MAX */) {
		block = read(fd, (char *)buffer + offset, count);

		if (block < 0)
			return block;
		if (!block)
			return offset;

		offset += block;
		count -= block;
	}

	return offset;
}

int glibc_assert(void)
{
	const struct rlimit rlim = {0, 0};
	if (setrlimit(RLIMIT_CORE, &rlim)) {
		perror("setrlimit");
		return TEST_SKIP_FAIL;
	}

	char argv0[0x40 * PAGE_SIZE], msg[sizeof(argv0) + PAGE_SIZE];
	ssize_t msg_size;
	memset(argv0, 'a', sizeof(argv0) - 1);
	__progname = argv0;

	for (int i = sizeof(argv0) - sizeof(MSG_SUFFIX); i > 0; i -= PAGE_SIZE + 2) {
		argv0[i] = 0;

		int pipefd[2];
		if (pipe(pipefd) == -1) {
			perror("pipe");
			return TEST_SKIP_FAIL;
		}

		int status;
		switch (fork()) {
		case -1:
			perror("fork");
			return TEST_SKIP_FAIL;
		case 0:
			close(pipefd[0]);
			if (dup2(pipefd[1], 2) == -1) {
				perror("dup2");
				return TEST_SKIP_FAIL;
			}
			close(pipefd[1]);
			assert(0);
			return TEST_SKIP_FAIL;
		default:
			close(pipefd[1]);
			msg_size = read_loop(pipefd[0], msg, sizeof(msg));
			if (msg_size == -1) {
				perror("read");
				return TEST_SKIP_FAIL;
			}
			close(pipefd[0]);
			if (wait(&status) == -1) {
				perror("wait");
				return TEST_SKIP_FAIL;
			}
/*
 * When assert() works correctly, it ends in an abort() and thus SIGABRT.
 *
 * If nothing writable is mapped after the "struct abort_msg_s *buf", then the
 * bug manifests itself as a segfault, which we detect here by lack of SIGABRT.
 *
 * If something writable is mapped there, then we may receive SIGABRT anyway
 * upon hitting a different code path with abort() reached from free(str), so
 * we proceed to check the message.
 */
			if (!WIFSIGNALED(status) || WTERMSIG(status) != SIGABRT) {
				if (WIFEXITED(status) && WEXITSTATUS(status) == TEST_SKIP_FAIL)
					return TEST_SKIP_FAIL;
				return TEST_FAIL;
			}
/*
 * Ensure the assert message appears and nothing extraneous appears after it,
 * in particular no "munmap_chunk(): invalid pointer", which happens when "str"
 * is mapped right after "buf".
 */
			if (msg_size < i || memcmp(msg + msg_size - (sizeof(MSG_SUFFIX) - 1), MSG_SUFFIX, sizeof(MSG_SUFFIX) - 1))
				return TEST_FAIL;
		}
	}

	return TEST_PASS;
}
