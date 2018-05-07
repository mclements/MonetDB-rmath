name = MonetDB-rmath
version = `sed -n 's/^Version:[ \t]*\(.*\)/\1/p' MonetDB-rmath.spec`

LIBDIR = $(shell pkg-config --variable=libdir monetdb5)

CC = cc

CFLAGS += $(shell pkg-config --cflags monetdb5)
CFLAGS += $(shell pkg-config --cflags libRmath)
LDFLAGS += $(shell pkg-config --libs monetdb5)
LDFLAGS += $(shell pkg-config --libs libRmath)

.SUFFIXES: .in

all: lib_rmath.so

lib_rmath.so: rmath.o
	$(CC) -fPIC -DPIC -o lib_rmath.so -shared rmath.o $(LDFLAGS) -Wl,-soname -Wl,lib_rmath.so

rmath.o: rmath.c
	$(CC) -fPIC -DPIC $(CFLAGS) -c rmath.c

M4       = m4
M4FLAGS  =
M4SCRIPT =

.in:
	${M4} ${M4FLAGS} ${M4SCRIPT} $< > $*

clean:
	rm -f *.o *.so

install: lib_rmath.so rmath.mal 74_rmath.sql
	mkdir -p $(DESTDIR)$(LIBDIR)/monetdb5/autoload $(DESTDIR)$(LIBDIR)/monetdb5/createdb
	cp rmath.mal lib_rmath.so $(DESTDIR)$(LIBDIR)/monetdb5
	cp ??_rmath.sql $(DESTDIR)$(LIBDIR)/monetdb5/createdb
	cp ??_rmath.mal $(DESTDIR)$(LIBDIR)/monetdb5/autoload

dist:
	tar -c -j -f $(name)-$(version).tar.bz2 --transform "s,^,$(name)-$(version)/," `hg files -X .hgtags`
