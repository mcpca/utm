VMAJ = 0
VMIN = 1
VERSION = $(VMAJ).$(VMIN)

PREFIX = /usr

CC ?= cc
CFLAGS += -std=c99 -pipe -O2 -march=native -mtune=native -Wall -Wextra -pedantic
INCLUDES = -I./include
# Flags for compiling as a shared library
SHCFLAGS = -fPIC
# Flags for compiling as a static library
STCFLAGS =

BUILDDIR=build

LDFLAGS = -shared -lm -Wl,-soname=libutm.so.$(VMAJ)

ifndef DEBUG
	CFLAGS += -O2 -DNDEBUG
else
	CFLAGS += -g
endif

all: libutm.a libutm.so.$(VERSION)

libutm.a: $(BUILDDIR)/static.o
	ar rcs $@ $^

libutm.so.$(VERSION): $(BUILDDIR)/shared.o
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILDDIR)/shared.o: utm.c | $(BUILDDIR)
	$(CC) -c $(CFLAGS) $(SHCFLAGS) $(INCLUDES) $< -o $@

$(BUILDDIR)/static.o: utm.c | $(BUILDDIR)
	$(CC) -c $(CFLAGS) $(STCFLAGS) $(INCLUDES) $< -o $@

test: test.c libutm.a
	$(CC) $(CFLAGS) -I./include -I./external/include $^ -lm -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

install: libutm.a libutm.so.$(VERSION)
	mkdir -p $(DESTDIR)$(PREFIX)/include/utm/
	cp -f ./include/utm/utm.h $(DESTDIR)$(PREFIX)/include/utm/utm.h
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	cp -f libutm.a $(DESTDIR)$(PREFIX)/lib/libutm.a
	cp libutm.so.$(VERSION) $(DESTDIR)$(PREFIX)/lib/libutm.so.$(VERSION)
	ln -sf $(DESTDIR)$(PREFIX)/lib/libutm.so.$(VERSION) $(DESTDIR)$(PREFIX)/lib/libutm.so
	ln -sf $(DESTDIR)$(PREFIX)/lib/libutm.so.$(VMAJ) $(DESTDIR)$(PREFIX)/lib/libutm.so
	ldconfig -n $(DESTDIR)$(PREFIX)/lib

uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/include/utm
	rm -f $(DESTDIR)$(PREFIX)/lib/libutm.{a,so*}

clean:
	rm -rf $(BUILDDIR)
	rm -f libutm.a libutm.so.$(VERSION) test

.PHONY: all clean install uninstall
