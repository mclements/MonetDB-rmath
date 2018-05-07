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
select rpois(100) from sys.generate_series(1,10); -- NOT repeated

select 10 as n, poissonci(10,1) as lower, poissonci(10,2) as upper
union all
select 0, poissonci(0,1), poissonci(0,2);
