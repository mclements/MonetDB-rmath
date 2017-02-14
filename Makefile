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
	cp gsl.mal lib_reverse.so $(DESTDIR)$(LIBDIR)/monetdb5
	cp 46_gsl.sql $(DESTDIR)$(LIBDIR)/monetdb5/createdb
	cp 73_gsl.mal $(DESTDIR)$(LIBDIR)/monetdb5/autoload
