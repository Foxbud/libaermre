SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .c .h .o .so



# Build directories.
prjdir = $(realpath $(CURDIR))
srcdir = $(prjdir)/src
incdir = $(prjdir)/include
builddir = $(prjdir)/build
docdir = $(prjdir)/docs

# Installation directories.
prefix = /usr/local
exec_prefix = $(prefix)
libdir = $(exec_prefix)/lib
includedir = $(prefix)/include

# Build files.
src = $(wildcard $(srcdir)/*.c)
pubinc = $(wildcard $(incdir)/aer*.h)
obj = $(src:.c=.o)
lib = $(builddir)/libaermre.so

# Program and flag defaults.
CFLAGS = -Wall -Wextra -O3 -g
ALL_CFLAGS = -I$(incdir) -m32 $(CFLAGS)
ALL_LDFLAGS = -shared -m32 -ldl $(LDFLAGS)
DOC = doxygen
INSTALL = install



.PHONY: lib
lib: $(lib)

$(lib): $(obj) $(builddir)
	$(CC) $(ALL_LDFLAGS) -o $@ $(obj)

%.o: %.c
	$(CC) -c $(ALL_CFLAGS) -o $@ $<

$(builddir):
	mkdir -p $@

$(docdir): $(pubinc)
	$(DOC) $(DOCFLAGS)

.PHONY: all
all: lib docs

.PHONY: docs
docs: $(docdir)

.PHONY: clean
clean:
	rm -rf $(obj) $(builddir) $(docdir)

.PHONY: install
install: lib $(pubinc)
	$(INSTALL) -Dt $(DESTDIR)$(libdir) $(lib)
	$(INSTALL) -Dt $(DESTDIR)$(includedir) -m 644 $(pubinc)

.PHONY: uninstall
uninstall:
	rm $(subst $(incdir),$(DESTDIR)$(includedir),$(pubinc))
	rm $(subst $(builddir),$(DESTDIR)$(libdir),$(lib))
