# CIQ RLC-H and Rocky Linux SIG/Security Test Suite

These are tests of security and robustness enhancements made in CIQ RLC-H
and Rocky Linux SIG/Security:

https://sig-security.rocky.page

This project is a contribution of CIQ.

## Output on Rocky Linux 9.5 with SIG/Security glibc override package

```
Test #1: asprintf(3) should set pointer to NULL on error ...	PASSED
Test #2: assert(3) should be unaffected by CVE-2025-0395 ...	PASSED
Test #3: fread(3)/fwrite(3) should detect integer overflow ...	PASSED
Test #4: qsort(3) robustness with nontransitive comparison ...	PASSED
```

## Output on stock Rocky Linux 9.5

```
Test #1: asprintf(3) should set pointer to NULL on error ...	FAILED
Test #2: assert(3) should be unaffected by CVE-2025-0395 ...	FAILED
Test #3: fread(3)/fwrite(3) should detect integer overflow ...	FAILED
Test #4: qsort(3) robustness with nontransitive comparison ...	FAILED
```
