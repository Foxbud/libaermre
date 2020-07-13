SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .c .h .o .so



prjdir = $(realpath $(CURDIR))
srcdir = $(prjdir)/src
incdir = $(prjdir)/include
builddir = $(prjdir)/build
docdir = $(prjdir)/docs

src = $(wildcard $(srcdir)/*.c)
inc = $(wildcard $(incdir)/*.h)
obj = $(src:.c=.o)
target = $(builddir)/aermre.so

CFLAGS = -Wall -Wextra -O3 -g
ALL_CFLAGS = -I$(incdir) -m32 $(CFLAGS)

ALL_LDFLAGS = -shared -m32 $(LDFLAGS)

DOC = doxygen



.PHONY: target
target: $(target)

$(target): $(obj) $(builddir)
	$(CC) $(ALL_LDFLAGS) -o $@ $(obj)

%.o: %.c
	$(CC) -c $(ALL_CFLAGS) -o $@ $<

$(builddir):
	mkdir -p $@

$(docdir): $(inc)
	$(DOC) $(DOCFLAGS)

.PHONY: all
all: target docs

.PHONY: docs
docs: $(docdir)

.PHONY: clean
clean:
	rm -rf $(obj) $(builddir) $(docdir)
