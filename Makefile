name = MonetDB-rmath
version = `sed -n 's/^Version:[ \t]*\(.*\)/\1/p' MonetDB-rmath.spec`

LIBDIR = `pkg-config --variable=libdir monetdb5`

CC = cc

CFLAGS = `pkg-config --cflags monetdb5` `pkg-config --cflags libRmath`
LDFLAGS = `pkg-config --libs monetdb5` `pkg-config --libs libRmath`

all: lib_rmath.so

lib_rmath.so: rmath.o
	$(CC) -fPIC -DPIC -o lib_rmath.so -shared rmath.o $(LDFLAGS) -Wl,-soname -Wl,lib_rmath.so

rmath.o: rmath.c
	$(CC) -fPIC -DPIC $(CFLAGS) -c rmath.c

clean:
	rm -f *.o *.so

install: lib_rmath.so
	mkdir -p $(DESTDIR)$(LIBDIR)/monetdb5/autoload $(DESTDIR)$(LIBDIR)/monetdb5/createdb
	cp rmath.mal lib_rmath.so $(DESTDIR)$(LIBDIR)/monetdb5
	cp ??_rmath.sql $(DESTDIR)$(LIBDIR)/monetdb5/createdb
	cp ??_rmath.mal $(DESTDIR)$(LIBDIR)/monetdb5/autoload

dist:
	tar -c -j -f $(name)-$(version).tar.bz2 --transform "s,^,$(name)-$(version)/," `hg files -X .hgtags`
