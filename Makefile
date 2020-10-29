SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .c .h .o .so



# Names.
libname = aermre
libnamev0 = lib$(libname).so
libnamev1 = $(libnamev0).1
libnamev3 = $(libnamev1).0.0dev
pubincname = aer

# Build directories.
prjdir = $(realpath $(CURDIR))
srcdir = $(prjdir)/src
incdir = $(prjdir)/include
pubincdir = $(incdir)/$(pubincname)
builddir = $(prjdir)/build
docdir = $(prjdir)/docs

# Installation directories.
prefix = /usr/local
exec_prefix = $(prefix)
libdir = $(exec_prefix)/lib
includedir = $(prefix)/include

# Build files.
src = $(wildcard $(srcdir)/*.c)
pubinc = $(wildcard $(pubincdir)/*.h)
dep = $(src:.c=.d)
obj = $(src:.c=.o)
lib = $(builddir)/$(libnamev3)

# Program and flag defaults.
DEP = $(CC)
DEPFLAGS = -M
ALL_DEPFLAGS = -m32 -I$(incdir) $(DEPFLAGS)
CFLAGS = -Wall -Wextra -Werror -O3
ALL_CFLAGS = -m32 -I$(incdir) $(CFLAGS)
LD = $(CC)
LDFLAGS = -rdynamic -ldl -l:libfoxutils.a
ALL_LDFLAGS = -m32 -shared -Wl,-soname,$(libnamev1) $(LDFLAGS)
DOC = doxygen
INSTALL = install



.PHONY: lib
lib: $(lib)

$(lib): $(obj)
	mkdir -p $(builddir)
	$(LD) -o $@ $(obj) $(ALL_LDFLAGS)

%.d: %.c
	$(DEP) $(ALL_DEPFLAGS) -MT $(@:.d=.o) -MF $@ $<

include $(dep)

%.o: %.c
	$(CC) -c $(ALL_CFLAGS) -o $@ $<

$(docdir): $(pubinc)
	$(DOC) $(DOCFLAGS)

.PHONY: all
all: lib docs

.PHONY: docs
docs: $(docdir)

.PHONY: clean
clean:
	rm -rf $(dep) $(obj) $(builddir) $(docdir)

.PHONY: install
install: install-symlinks install-headers

.PHONY: install-lib
install-lib: lib
	$(INSTALL) -Dt $(DESTDIR)$(libdir) $(dlib)

.PHONY: install-symlinks
install-symlinks: install-lib
	ln -srf $(DESTDIR)$(libdir)/$(libnamev3) $(DESTDIR)$(libdir)/$(libnamev1)
	ln -srf $(DESTDIR)$(libdir)/$(libnamev3) $(DESTDIR)$(libdir)/$(libnamev0)

.PHONY: install-headers
install-headers: $(pubinc)
	$(INSTALL) -Dt $(DESTDIR)$(includedir)/$(pubincname) -m 644 $(pubinc)

.PHONY: uninstall
uninstall: uninstall-lib uninstall-headers

.PHONY: uninstall-lib
uninstall-lib: uninstall-symlinks
	rm -f $(DESTDIR)$(libdir)/$(libnamev3)

.PHONY: uninstall-symlinks
uninstall-symlinks:
	rm -f $(DESTDIR)$(libdir)/$(libnamev0) $(DESTDIR)$(libdir)/$(libnamev1)

.PHONY: uninstall-headers
uninstall-headers:
	rm -rf $(DESTDIR)$(includedir)/$(pubincname)
