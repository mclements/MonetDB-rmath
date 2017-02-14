name = MonetDB-gsl
version = `sed -n 's/^Version:[ \t]*\(.*\)/\1/p' MonetDB-gsl.spec`

LIBDIR = `pkg-config --variable=libdir monetdb5`

CC = cc

CFLAGS = `pkg-config --cflags monetdb5`
LDFLAGS = `pkg-config --libs monetdb5`

all: lib_gsl.so

lib_gsl.so: gsl.o
	$(CC) -fPIC -DPIC -o lib_gsl.so -shared gsl.o $(LDFLAGS) -Wl,-soname -Wl,lib_gsl.so

gsl.o: gsl.c
	$(CC) -fPIC -DPIC $(CFLAGS) -c gsl.c

clean:
	rm -f *.o *.so

install: lib_gsl.so
	mkdir -p $(DESTDIR)$(LIBDIR)/monetdb5/{autoload,createdb}
	cp gsl.mal lib_gsl.so $(DESTDIR)$(LIBDIR)/monetdb5
	cp ??_gsl.sql $(DESTDIR)$(LIBDIR)/monetdb5/createdb
	cp ??_gsl.mal $(DESTDIR)$(LIBDIR)/monetdb5/autoload

dist:
	tar -c -j -f $(name)-$(version).tar.bz2 --transform "s,^,$(name)-$(version)/," `hg files -X .hgtags`
