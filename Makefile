CC = gcc
LD = $(CC)
RM = rm -f
MKDIR = mkdir -p
INSTALL = install -c
# -pie is a linker option. but let's have it in CFLAGS to ensure -fPIE and -pie
# are ever overridden or not together
CFLAGS = -std=c99 -Wall -W -O2 -fomit-frame-pointer -fPIE -pie -fstack-protector-strong -D_FORTIFY_SOURCE=2
# Include CFLAGS into LDFLAGS for two reasons:
# 1. "For predictable results, you must also specify the same set of options
#    used for compilation (-fpie, -fPIE, or model suboptions) when you specify
#    this linker option." (as gcc man page says about "-pie")
# 2. In case the package only overrides CFLAGS, we want the distro's hardening
#    or other flags to also propagate to the linker.
LDFLAGS = -s -Wl,-z,defs -Wl,-z,relro -Wl,-z,now $(CFLAGS)

DESTDIR =
PREFIX = /usr/local
SBINDIR = $(PREFIX)/sbin

PROJ = rock-hard-tests-glibc
OBJS = glibc-all.o glibc-asprintf.o glibc-assert.o glibc-freadwrite.o glibc-qsort.o

all: $(PROJ)

$(PROJ): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(OBJS): glibc-all.h

.c.o:
	$(CC) $(CFLAGS) -c $*.c

install: $(PROJ)
	$(MKDIR) -m 755 $(DESTDIR)$(SBINDIR)
	$(INSTALL) -m 700 $(PROJ) $(DESTDIR)$(SBINDIR)/

uninstall:
	$(RM) $(DESTDIR)$(SBINDIR)/$(PROJ)

clean:
	$(RM) $(PROJ) $(OBJS)
