-- (async-shell-command "monetdbd start ~/work/mydbfarm")
-- (sql-monetdb)

SELECT sys.r_pchisq(20.0, 5.0);

SELECT sys.r_pchisq(20.0, NULL);

SELECT sys.r_pchisq(NULL, 5.0);

SELECT sys.r_pchisq(-1, 5.0); -- there is currently no guard against out-of-range inputs

SELECT sys.r_pchisq(20.0, 1.0);

CREATE TABLE chi2(a double, b double);

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0);

SELECT r_pchisq(a, b) FROM chi2;

SELECT r_pchisq(a, 6.0) FROM chi2;

SELECT r_pchisq(19.0, b) FROM chi2;

INSERT INTO chi2 VALUES (20.0, NULL);

SELECT r_pchisq(a, b) FROM chi2;

SELECT r_pchisq(a, 6.0) FROM chi2;

SELECT r_pchisq(19.0, b) FROM chi2;

DELETE FROM chi2;

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0),
                        (NULL, 5.0);

SELECT r_pchisq(a, b) FROM chi2;

SELECT r_pchisq(a, 6.0) FROM chi2;

SELECT r_pchisq(19.0, b) FROM chi2;

DELETE FROM chi2;

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0),
                        (-1, 5.0);

SELECT r_pchisq(a, b) FROM chi2;

SELECT r_pchisq(a, 6.0) FROM chi2;

SELECT r_pchisq(19.0, b) FROM chi2;

DELETE FROM chi2;

drop table chi2;
create table chi2 as select value/10000.0 as value from sys.generate_series(cast(1 as integer),100000);
select count(*) from (select r_pchisq(value,2) from chi2) as t;
select count(*) from (select r_pnorm(value,0,1) from chi2) as t;
drop table chi2;

select r_set_seed(1,2);
select r_runif(0.0,1.0);
select r_runif(0.0,1.0);
select r_set_seed(1,2);
select r_runif(0.0,1.0);
select r_runif(0.0,1.0);

select r_rpois(value*0+100) from sys.generate_series(1,10); -- ok
select value, r_rpois(100) from sys.generate_series(1,10);  -- repeated
select r_rpois(100) from sys.generate_series(1,10);         -- repeated

select 10 as n, r_poisson_ci(10,1) as lower, r_poisson_ci(10,2) as upper
union all
select 0, r_poisson_ci(0,1), r_poisson_ci(0,2);

-- scalar function using R
drop function poisson_ci2(double,integer);
create function poisson_ci2(y double,boundary integer) returns double language r {sapply(y,function(yi) poisson.test(yi)$conf.int[boundary])};
-- table returning function using R
drop function poisson_ci3(double);
create function poisson_ci3(y double) returns table (lci double, uci double) language r {val = t(sapply(y,function(yi) {test = poisson.test(yi)})); data.frame(lci=val[,1],uci=val[,2])};
-- table returning function using C code
drop function poisson_ci4(double);
create function poisson_ci4(y double) returns table (lci double, uci double) begin return table(select r_poisson_ci(y,1) as lci, r_poisson_ci(y,2) as uci); end;
-- vectorised R code
drop function poisson_ci5(double);
create function poisson_ci5(y double) returns table (lci double, uci double) language r {alpha=0.025; data.frame(lci=ifelse(y==0,0,qgamma(alpha,y)), uci=qgamma(1-alpha,y+1))};
--
drop function poisson_test2(double,double);
create function poisson_test2(y double,e double) returns double language r {mapply(function(yi,ei) poisson.test(yi,ei)$p.value,y,e)};
drop function poisson_test3(double,double);
create function poisson_test3(y double,e double) returns table(lci double, uci double, pvalue double) language r {data.frame(t(mapply(function(yi,ei) {test=poisson.test(yi,ei); c(lci=test$conf.int[1], uci=test$conf.int[2], pvalue=test$p.value)},y,e)))};
select poisson_test3(10,12).*;

\t clock
drop table vals;
create table vals as select cast(value as double) as value, cast(value as double)*1.1 as value2, 1.0 as r, 2 as alt  from generate_series(1.0,10001.0,1.0);
select sum(lci), sum(uci) from (select r_poisson_ci(value,1) as lci, r_poisson_ci(value,2) as uci from vals) as t; -- 28ms FASTEST (slightly faster than vectorised R)
select sum(poisson_ci2(value,1)), sum(poisson_ci2(value,2)) from vals; -- 1.6s
select sum(lci), sum(uci) from poisson_ci3((select value from vals)); -- 790ms
select sum(lci), sum(uci) from poisson_ci4((select value from vals)); -- 1.1s?? SLOW
select sum(lci), sum(uci) from poisson_ci5((select value from vals)); -- 40ms FAST
select sum(lci), sum(uci) from vals, lateral poisson_ci4(vals.value) as t2; -- 1.3s SLOW (lateral)
--
select sum(poisson_test(value,value2)) from vals; -- 5.6s
select sum(pvalue) from poisson_test3((select value,value2 from vals)); -- 8.8s


drop table test;
create table test as select * from poisson_ci4((select value from generate_series(1.0,10000.0,1.0))); -- 1.2s
drop table test;

select poisson_ci(10,1) as lci, poisson_ci(10,2); -- ok
explain select poisson_ci(value,1) as lci from generate_series(10.0,12.0,1.0); -- no!
select poisson_ci(value,2) as uci from generate_series(10.0,12.0,1.0); -- ok
select poisson_ci(value,1,0.95) as lci, poisson_ci(value,2,0.95) as uci from generate_series(10.0,11.0,1.0); -- ??
select poisson_ci(value,1) as lci, poisson_ci(value,2) as uci from generate_series(10.0,11.0,1.0); -- ??
select poisson_ci(value,1) as lci, poisson_ci(value,2) from (select 10 as value) as x;
select poisson_ci(value,1) as lci, poisson_ci(value,2) from (select 10 as value union select 11 as value) as x;

drop table vals;
create table vals as select cast(value as double) as generate_series from generate_series(1.0,10.0,1.0);
explain select r_poisson_ci(value,1,0.95) as lci, r_poisson_ci(value,2,0.95) as uci from generate_series(10.0,11.0,1.0);
explain select r_poisson_ci(value,1) as lci, r_poisson_ci(value,2) as uci from generate_series(10.0,11.0,1.0);
explain select r_poisson_ci(value,1) as lci, r_poisson_ci(value,2) from (select 10 as value union select 11 as value) as x;
explain select *  from vals, lateral poisson_ci4(vals.value) as t2;


drop function trf1(double); 
create function trf1(x double) returns table(a double, b double)
  begin
    return table(select x as a, 2*x as b);
  end;
select * from trf1(cast(2.0 as double)); -- ok
drop table test;
create table test as select cast(value as double) as value from generate_series(0.0,10001.0,1.0);
explain select sum(a), sum(b) from trf1((select value from test)) as t2;
select sum(a), sum(b) from test, lateral trf1(test.value) as t2;

select sum(a), sum(b) from trf1((select value from test)) as t2;

drop function trf1(double); 
create function trf1(x double) returns table(a double, b double)
  begin
    return table(select x as a, 2*x as b);
  end;
select * from trf1(cast(2.0 as double)); -- ok
drop table test;
create table test as select cast(value as double) as value from generate_series(0.0,5.0,1.0);
select sum(a), sum(b) from trf1((select value from test)) as t2;
select sum(a), sum(b) from test, lateral trf1(test.value) as t2;
select sum(a), sum(b) from (select trf1(value).* from test);

explain select * from
(select *, row_number() over() as rid from test) as t1
natural join
(select *, row_number() over() as rid from trf1((select value from test))) as t2;
explain select * from test, lateral trf1(test.value) as t2;

-- test data
declare cl double;
set cl=0.95;
create or replace table test as select cast(value as double) as value, cl as conflevel from generate_series(0.0,5.0,1.0);
-- a scalar function *can* return multiple values
select poisson_cis(10,cl).*; -- ok
select * from poisson_cis(10,cl); -- ok
-- table-returning function in FROM
select * from poisson_cis((select value, cl from test)); -- ok
-- use ROW_NUMBER to join the input data with the results
select * from
(select *, row_number() over() as rid from test) as t1
natural join
(select *, row_number() over() as rid from poisson_cis((select value,cl from test))) as t2; -- ok
-- use LATERAL to join the input data with the results
select * from test, lateral poisson_cis(test.value, cl) as t2; -- ok
select * from test, lateral poisson_cis(test.value, test.conflevel) as t2; -- ok
create or replace function poisson_cis2(x double, conflevel double)
  returns table(lci double, uci double)
  begin
   return select poisson_ci(x,1,conflevel), poisson_ci(x,1,conflevel);
  end;
select poisson_cis2(10,cl).*;
select * from poisson_cis2(10,cl);

-- Errors
select poisson_cis(test.value,cl).* from test; -- Error: no such column 'test.value'
select poisson_cis(value,cl).* from test; -- identifier 'value' unknown
select poisson_cis(test.value,test.conflevel).* from test; -- Error: no such column 'test.value'
select poisson_cis(value,test.conflevel).* from test; -- identifier 'value' unknown
select poisson_cis2(value,cl).* from test; -- Error: identifier 'value' unknown
select poisson_cis2(test.value,cl).* from test; -- Error: no such column 'test.value'
-- Should table-returning functions in the select statement only return one row?

explain select * from poisson_cis((select value, cl from test)); -- vectorised
explain select * from poisson_cis((select value, conflevel from test)); -- vectorised
explain select * from
(select *, row_number() over() as rid from test) as t1
natural join
(select *, row_number() over() as rid from poisson_cis((select value,conflevel from test))) as t2; -- vectorised
explain select * from test, lateral poisson_cis(test.value, cl) as t2; -- NOT vectorised
explain select * from test, lateral poisson_cis(test.value, test.conflevel) as t2; -- NOT vectorised

explain select * from test, lateral poisson_cis(test.value, cl) as t2;

-- clean example
-- test data
drop table test;
create table test as select cast(value as double) as value from generate_series(0,5);
\d test
-- test function
drop function cis(double,double);
create function cis(mu double, se double) returns table(lci double, uci double)
  begin return select mu-1.96*se, mu+1.96*se;
  end;
-- table-returning functions ok
select * from cis((select value, 1.0 from test));
-- re-join using row_number()
select * from
(select *, row_number() over() as rid from test) as t1
natural join
(select *, row_number() over() as rid from cis((select value,1.0 from test))) as t2;
-- re-join using lateral
select * from test, lateral cis(test.value, cast(1.0 as double)) as t2;
-- table-returning function in select
select cis(2,1).*; -- ok
select cis(2,1).* from test; -- Error: no such operator 'cis'
select cis(value,1).* from test; -- Error: identifier 'value' unknown
-- lateral is not "vectorised" -- there would be a loop even if a BAT function were available
-- How to better join results? (Email sent 2018-05-25)

-- example of using a random numbers from R
-- R: unsigned <- function(seed) ifelse(seed < 0, seed + 2^32, seed)
-- R: signed <- function(seed) ifelse(seed>2^31, seed-2^32, seed)
drop function rpois2(double,double);
drop function set_seed2(integer);
drop function get_seed3();
drop procedure set_seed();
drop procedure save_seed();
drop aggregate set_seed3(integer);
drop table r_random_seed;
drop table r_random_seed_test;
--
create function rpois2(n double, mu double) returns table(y int) language r {data.frame(y=rpois(n,mu))};
create function set_seed2(seed integer) returns boolean language r { set.seed(seed); TRUE };
create function get_seed3() returns table(seed int) language r { data.frame(seed=.Random.seed)};
create aggregate set_seed3(seed integer) returns boolean language r { .Random.seed <<- ifelse(seed<0,seed+2^32,seed) ; TRUE};
create table r_random_seed (seed integer);
create table r_random_seed_test (result boolean);
create procedure save_seed()
  begin
    delete from r_random_seed;
    insert into r_random_seed select seed from get_seed3();
  end;
create procedure set_seed()
  begin
    delete from r_random_seed_test;
    insert into r_random_seed_test select set_seed3(seed) from r_random_seed;
  end;
select set_seed2(10);
select * from rpois2(5.0,10.0);
select set_seed2(10);
select * from rpois2(5.0,10.0);
select set_seed2(10);
call save_seed();
call set_seed();
select * from rpois2(5.0,10.0); -- ?? 
call set_seed();
select * from rpois2(5.0,10.0); -- ??

create function set_random_seed(seed integer)
  returns boolean language r { set.seed(seed); TRUE };
create function rpois(n integer, mu double)
  returns table(y integer) language r {data.frame(y=rpois(n,mu))};
select set_random_seed(10);
select * from rpois(5,10);


-- select * from generate_series(0.0,10.0,1.0) as g, lateral rpois(g.value);
select * from generate_series(0.0,10.0,1.0) as g, rpois(g.value);

drop table vals;
create table vals as select cast(value as double) as value from generate_series(1.0,10.0,1.0);
select * from vals; -- ok
select poisson_ci4(10.0).*; -- ok
select poisson_ci4(value).* from vals; -- fails: identifier 'value' unknown
create table vals2 as select * from vals, lateral poisson_ci4(vals.value) as t2; -- ok (slow iteration)

explain select * from poisson_ci4((select value from vals));



drop table test;
create table test as select 1 as boundary, value as y, 0.95 as conflevel from generate_series(0.0,10.0,1.0);
explain select poisson_ci(y,boundary,conflevel) from test;
select poisson_ci(y,boundary,conflevel) from test;
select poisson_ci(y,1) from test;


select * from poisson_ci3(10); 
select * from poisson_ci4(10); 
select * from poisson_ci3((select value from generate_series(1.0,10.0,1.0)));
select * from poisson_ci4((select value from generate_series(0.0,10.0,1.0)));   

explain select sum(poisson_ci(value,1)), sum(poisson_ci(value,2)) from generate_series(1.0,10000.0,1.0);
explain select * from poisson_ci4((select value from generate_series(0.0,10000.0,1.0)));
explain create table test as select * from poisson_ci4((select value from generate_series(1.0,10000.0,1.0)));

with t as (select * from generate_series(1.0,10.0,1.0))
select * from poisson_ci4((select value from t)) union select * from t;

drop function multiply(integer);
CREATE FUNCTION multiply(input INTEGER) 
RETURNS INTEGER
LANGUAGE C {
    result->initialize(result, input.count);
    // loop over the input values
    for(size_t i = 0; i < input.count; i++) {
        if (input.is_null(input.data[i])) {
            // handle NULL values
            result->data[i] = result->null_value;
        } else {
            // handle regular values
            result->data[i] = input.data[i] * 2;
        }
    }
};
drop table integers;
CREATE TABLE integers(i INTEGER);
INSERT INTO integers VALUES (1), (2), (NULL), (3), (4);
SELECT i, multiply(i) FROM integers;

drop function mult2(integer,integer);
CREATE FUNCTION mult2(input1 INTEGER, input2 INTEGER) 
RETURNS INTEGER
LANGUAGE C {
    size_t n1 = input1.count>1;
    size_t n2 = input2.count>1;
    size_t n = input1.count>input2.count ? input1.count : input2.count;
    result->initialize(result, n);
    // loop over the input values
    for(size_t i = 0; i < n; i++) {
        if (input1.is_null(input1.data[i*n1]) || input2.is_null(input2.data[i*n2])) {
            // handle NULL values
            result->data[i] = result->null_value;
        } else {
            // handle regular values
            result->data[i] = input1.data[i*n1]*input2.data[i*n2];
        }
    }
};
drop table integers;
CREATE TABLE integers(i INTEGER);
INSERT INTO integers VALUES (1), (2), (NULL), (3), (4);
SELECT i, mult2(i,i) FROM integers;
SELECT i, mult2(3,i) FROM integers;
SELECT i, mult2(i,4) FROM integers;
SELECT i, mult2(5,4) FROM integers;

explain SELECT i, mult2(i,i) FROM integers;
explain SELECT i, mult2(3,i) FROM integers;
explain SELECT i, mult2(i,4) FROM integers;
explain SELECT i, mult2(5,4) FROM integers;

drop function mult3(integer,integer,integer);
CREATE FUNCTION mult3(input1 INTEGER, input2 INTEGER, input3 integer) 
RETURNS INTEGER
LANGUAGE C {
    size_t n1 = input1.count>1;
    size_t n2 = input2.count>1;
    size_t n3 = input3.count>1;
    size_t n = input1.count>input2.count ? input1.count : input2.count;
    if (input3.count>n) n = input3.count;
    result->initialize(result, n);
    // loop over the input values
    for(size_t i = 0; i < n; i++) {
        if (input1.is_null(input1.data[i*n1]) ||
	   input2.is_null(input2.data[i*n2])  ||
	   input3.is_null(input3.data[i*n3])) {
            // handle NULL values
            result->data[i] = result->null_value;
        } else {
            // handle regular values
            result->data[i] = input1.data[i*n1]*input2.data[i*n2]*input3.data[i*n3];
        }
    }
};
drop table integers;
CREATE TABLE integers(i INTEGER);
INSERT INTO integers VALUES (1), (2), (NULL), (3), (4);
SELECT i, mult3(i,i,i) FROM integers;
SELECT i, mult3(3,i,1) FROM integers;
SELECT i, mult3(i,4,1) FROM integers;
SELECT i, mult3(5,4,1) FROM integers;


drop function myexp();
CREATE FUNCTION myexp()
RETURNS double
LANGUAGE C {
#define MATHLIB_STANDALONE
#include "Rmath.h"
#pragma CFLAGS -I/usr/share/R/include
#pragma LDFLAGS -lRmath -lm
    result->initialize(result, 1);
    result->data[0] = M_E;
};
drop table integers;
CREATE TABLE integers(i INTEGER);
INSERT INTO integers VALUES (1), (2), (NULL), (3), (4);
select myexp();
SELECT i, myexp() FROM integers;

/*


*/
