typedef enum {
	TEST_PASS = 42,
	TEST_FAIL = 142,
	TEST_SKIP_NA = 242,
	TEST_SKIP_FAIL = 255
} test_result;

extern int glibc_asprintf(void);
extern int glibc_assert(void);
extern int glibc_freadwrite(void);
extern int glibc_qsort(void);
