name     = MonetDB-rmath
version  = `sed -n 's/^Version:[ \t]*\(.*\)/\1/p' MonetDB-rmath.spec`

CC       = cc
M4       = m4
M4FLAGS  =
M4SCRIPT =

LIBDIR   = $(shell pkg-config --variable=libdir monetdb5)
# CFLAGS  += -g -Wall
CFLAGS  += $(shell pkg-config --cflags libRmath)
LDFLAGS += $(shell pkg-config --libs libRmath)

all: 74_rmath.sql

74_rmath.sql: 74_rmath.sql.in
	${M4} ${M4FLAGS} ${M4SCRIPT} 74_rmath.sql.in > temp
	${M4} ${M4FLAGS} -DDCFLAGS="${CFLAGS}" -DDLDFLAGS="${LDFLAGS}" changecom1.m4 temp > 74_rmath.sql

clean:
	rm -f 74_rmath.sql

install: 74_rmath.sql
	mkdir -p $(DESTDIR)$(LIBDIR)/monetdb5/createdb
	cp 74_rmath.sql $(DESTDIR)$(LIBDIR)/monetdb5/createdb

dist:
	tar -c -j -f $(name)-$(version).tar.bz2 --transform "s,^,$(name)-$(version)/," `hg files -X .hgtags`

sql:
	monetdbd start ~/work/mydbfarm || true
	mclient -d testt

mal:
	monetdbd start ~/work/mydbfarm || true
	mclient -l mal testt

start:
	monetdbd start ~/work/mydbfarm || true

stop:
	monetdbd stop ~/work/mydbfarm || true

test:
	monetdbd start ~/work/mydbfarm || true
	monetdb destroy -f testt || true
	monetdb create testt
	monetdb release testt
	monetdb set embedr=yes testt
	monetdb set embedc=yes testt
	mclient -d testt -s "select r_pnorm(1.96, 0, 1); create table temp (x double); insert into temp values (0.1); insert into temp values (0.2); select r_qgamma(x,2.0,1.0,1,0), r_qgamma(x*2,2,1) from temp;"
