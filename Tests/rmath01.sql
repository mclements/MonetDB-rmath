-- (async-shell-command "monetdbd start ~/work/mydbfarm")
-- (sql-monetdb)

SELECT sys.pchisq(20.0, 5.0);

SELECT sys.pchisq(20.0, NULL);

SELECT sys.pchisq(NULL, 5.0);

SELECT sys.pchisq(-1, 5.0); -- there is currently no guard against out-of-range inputs

SELECT sys.pchisq(20.0, 1.0);

CREATE TABLE chi2(a double, b double);

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0);

SELECT pchisq(a, b) FROM chi2;

SELECT pchisq(a, 6.0) FROM chi2;

SELECT pchisq(19.0, b) FROM chi2;

INSERT INTO chi2 VALUES (20.0, NULL);

SELECT pchisq(a, b) FROM chi2;

SELECT pchisq(a, 6.0) FROM chi2;

SELECT pchisq(19.0, b) FROM chi2;

DELETE FROM chi2;

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0),
                        (NULL, 5.0);

SELECT pchisq(a, b) FROM chi2;

SELECT pchisq(a, 6.0) FROM chi2;

SELECT pchisq(19.0, b) FROM chi2;

DELETE FROM chi2;

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0),
                        (-1, 5.0);

SELECT pchisq(a, b) FROM chi2;

SELECT pchisq(a, 6.0) FROM chi2;

SELECT pchisq(19.0, b) FROM chi2;

DELETE FROM chi2;

drop table chi2;
create table chi2 as select value/10000.0 as value from sys.generate_series(cast(1 as integer),100000);
select count(*) from (select pchisq(value,2) from chi2) as t;
select count(*) from (select pnorm(value,0,1) from chi2) as t;
drop table chi2;

select set_seed(1,2);
select runif(0.0,1.0);
select runif(0.0,1.0);
select set_seed(1,2);
select runif(0.0,1.0);
select runif(0.0,1.0);

select rpois(value*0+100) from sys.generate_series(1,10); -- ok
select value, rpois(100) from sys.generate_series(1,10);  -- repeated
select rpois(100) from sys.generate_series(1,10);         -- repeated

select 10 as n, poissonci(10,1) as lower, poissonci(10,2) as upper
union all
select 0, poissonci(0,1), poissonci(0,2);

-- scalar function using R
drop function poisson_ci2(double,integer);
create function poisson_ci2(y double,boundary integer) returns double language r {sapply(y,function(yi) poisson.test(yi)$conf.int[boundary])};
-- table returning function using R
drop function poisson_ci3(double);
create function poisson_ci3(y double) returns table (lci double, uci double) language r {val = t(sapply(y,function(yi) {test = poisson.test(yi)})); data.frame(lci=val[,1],uci=val[,2])};
-- table returning function using C code
drop function poisson_ci4(double);
create function poisson_ci4(y double) returns table (lci double, uci double) begin return table(select poisson_ci(y,1) as lci, poisson_ci(y,2) as uci); end;
-- vectorised R code
drop function poisson_ci5(double);
create function poisson_ci5(y double) returns table (lci double, uci double) language r {alpha=0.025; data.frame(lci=ifelse(y==0,0,qgamma(alpha,y)), uci=qgamma(1-alpha,y+1))};
--
drop function poisson_test2(double,double);
create function poisson_test2(y double,e double) returns double language r {mapply(function(yi,ei) poisson.test(yi,ei)$p.value,y,e)};
drop function poisson_test3(double,double);
create function poisson_test3(y double,e double) returns table(lci double, uci double, pvalue double) language r {data.frame(t(mapply(function(yi,ei) {test=poisson.test(yi,ei); c(lci=test$conf.int[1], uci=test$conf.int[2], pvalue=test$p.value)},y,e)))};
select poisson_test3(10,12).*;

drop table vals;
create table vals as select cast(value as double) as value, cast(value as double)*1.1 as value2, 1.0 as r, 2 as alt  from generate_series(1.0,10001.0,1.0);
select sum(lci), sum(uci) from (select poisson_ci(value,1) as lci, poisson_ci(value,2) as uci from vals) as t; -- 28ms FASTEST (slightly faster than vectorised R)
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
explain select poisson_ci(value,1,0.95) as lci, poisson_ci(value,2,0.95) as uci from generate_series(10.0,11.0,1.0);
explain select poisson_ci(value,1) as lci, poisson_ci(value,2) as uci from generate_series(10.0,11.0,1.0);
explain select poisson_ci(value,1) as lci, poisson_ci(value,2) from (select 10 as value union select 11 as value) as x;
explain select *  from vals, lateral poisson_ci4(vals.value) as t2;


drop function trf1(double); 
create function trf1(x double) returns table(a double, b double)
  begin
    return table(select x as a, 2*x as b);
  end;
select * from trf1(cast(2.0 as double)); -- ok
select * from generate_series(0.0,5.0,1.0) as t, lateral trf1(cast(t.value as double)) as t2;



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
