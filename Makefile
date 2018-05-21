.SUFFIXES: .in

name     = MonetDB-rmath
version  = `sed -n 's/^Version:[ \t]*\(.*\)/\1/p' MonetDB-rmath.spec`

CC       = cc
M4       = m4
M4FLAGS  =
M4SCRIPT =

LIBDIR   = $(shell pkg-config --variable=libdir monetdb5)
CFLAGS  += -g -Wall
CFLAGS  += $(shell pkg-config --cflags monetdb5)
CFLAGS  += $(shell pkg-config --cflags libRmath)
LDFLAGS += $(shell pkg-config --libs monetdb5)
LDFLAGS += $(shell pkg-config --libs libRmath)

all: lib_rmath.so

.in:
	${M4} ${M4FLAGS} ${M4SCRIPT} $< > $*

lib_rmath.so: rmath.o
	$(CC) -fPIC -DPIC -o lib_rmath.so -shared rmath.o $(LDFLAGS) -Wl,-soname -Wl,lib_rmath.so

rmath.o: rmath.c
	$(CC) -fPIC -DPIC $(CFLAGS) -c rmath.c

clean:
	rm -f *.o *.so rmath.mal 74_rmath.sql

install: lib_rmath.so rmath.mal 74_rmath.sql
	mkdir -p $(DESTDIR)$(LIBDIR)/monetdb5/autoload $(DESTDIR)$(LIBDIR)/monetdb5/createdb
	cp rmath.mal lib_rmath.so $(DESTDIR)$(LIBDIR)/monetdb5
	cp ??_rmath.sql $(DESTDIR)$(LIBDIR)/monetdb5/createdb
	cp ??_rmath.mal $(DESTDIR)$(LIBDIR)/monetdb5/autoload

dist:
	tar -c -j -f $(name)-$(version).tar.bz2 --transform "s,^,$(name)-$(version)/," `hg files -X .hgtags`

run:
	monetdbd start ~/work/mydbfarm || true
	monetdb destroy -f testt || true
	monetdb create testt
	monetdb release testt
	mclient -d testt

mal:
	mclient -l mal testt

start:
	monetdbd start ~/work/mydbfarm || true

test:
	monetdbd start ~/work/mydbfarm || true
	monetdb destroy -f testt || true
	monetdb create testt
	monetdb release testt
	monetdb set embedr=yes testt
	mclient -d testt -s "select pnorm(1.96, 0, 1); create table temp (x double); insert into temp values (0.1); insert into temp values (0.2); select qgamma(x,2.0,1.0,1,0), qgamma(x*2,2.0,1.0,1,0) from temp; select poisson_ci(10,1), poisson_ci(10,2); select poisson_ci(value,1), poisson_ci(value,2) from (select 10 as value union select 11 as value) as t; select poisson_test(value,value*2,1.0,2) as pvalue from generate_series(cast(0.0 as double),5.0,1.0);"
	# monetdbd stop ~/work/mydbfarm
