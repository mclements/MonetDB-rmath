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
create function poisson_ci3(y double) returns table (lci double, uci double) language r {val = t(sapply(y,function(yi) {poisson.test(yi)$conf.int})); data.frame(lci=val[,1],uci=val[,2])};
-- table returning function using C code
drop function poisson_ci4(double);
create function poisson_ci4(y double) returns table (lci double, uci double) begin return table(select poisson_ci(y,1) as lci, poisson_ci(y,2) as uci); end;

select sum(lci), sum(uci) from (select poisson_ci(value,1) as lci, poisson_ci(value,2) as uci from generate_series(1.0,10000.0,1.0)) as t; -- 32ms
select sum(lci), sum(uci) from (select poisson_ci2(value,1) as lci, poisson_ci2(value,2) as uci from generate_series(1.0,10000.0,1.0)) as t; -- 1.6s
select sum(lci), sum(uci) from poisson_ci3((select value from generate_series(1.0,10000.0,1.0))); -- 790ms
select sum(lci), sum(uci) from poisson_ci4((select value from generate_series(1.0,10000.0,1.0))); -- 1.1s?? SLOW

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

explain select poisson_ci(value,1,0.95) as lci, poisson_ci(value,2,0.95) as uci from generate_series(10.0,11.0,1.0);
explain select poisson_ci(value,1) as lci, poisson_ci(value,2) as uci from generate_series(10.0,11.0,1.0);
explain select poisson_ci(value,1) as lci, poisson_ci(value,2) from (select 10 as value union select 11 as value) as x;

select * from poisson_ci3(10); 
select * from poisson_ci4(10); 
select * from poisson_ci3((select value from generate_series(1.0,10.0,1.0)));
select * from poisson_ci4((select value from generate_series(0.0,10.0,1.0)));   

explain select sum(poisson_ci(value,1)), sum(poisson_ci(value,2)) from generate_series(1.0,10000.0,1.0);
explain select * from poisson_ci4((select value from generate_series(0.0,10000.0,1.0)));
explain create table test as select * from poisson_ci4((select value from generate_series(1.0,10000.0,1.0)));

with t as (select * from generate_series(1.0,10.0,1.0))
select * from poisson_ci4((select value from t)) union select * from t;
