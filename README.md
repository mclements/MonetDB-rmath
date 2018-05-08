

# MonetDB-rmath: MonetDB extension to use the Rmath library.

An early version for MonetDB-rmath is now available. It includes approximately 90% of the functions in [Rmath.h](http://docs.rexamine.com/R-devel/Rmath_8h_source.html), including [rpdq] (norm|unif|gamma|beta|lnorm|chisq|nchisq|f|t|binom|cauchy|exp|geom|hyper|nbinom|pois|weibull|logis|wilcox|signrank) and another 40 functions. The current implementation uses m4 for code generation and requires the r-mathlib package under Debian.

One can clone the GitHub repository and install using:
``` bash
git clone https://github.com/mclements/MonetDB-rmath.git
cd MonetDB-rmath
make
sudo make install
```
You will then need to use a fresh database. An example script is:
``` SQL
-- calculate the quantile and cumulative distribution function for a unit normals
select qnorm(0.025,0,1), pnorm(-1.96,0,1);

+--------------------------+--------------------------+
| L2                       | L4                       |
+==========================+==========================+
|      -1.9599639845400538 |     0.024997895148220428 |
+--------------------------+--------------------------+

-- calculate E(Z^2) for Z~Normal(0,1)
select sum(R_pow(value,2)*0.01*dnorm(value,0,1)) from
  sys.generate_series(-5.0,5.0,0.01);

+--------------------------+
| L7                       |
+==========================+
|       0.9999845566522494 |
+--------------------------+

-- random numbers
select set_seed(1,2);
  
select rpois(value*0+100) from sys.generate_series(1,10); -- ok
  
+--------------------------+
| L5                       |
+==========================+
|                       87 |
|                       90 |
|                       96 |
|                      117 |
|                       87 |
|                       99 |
|                       95 |
|                       86 |
|                      116 |
+--------------------------+

select rpois(100) from sys.generate_series(1,3);  -- repeats the values:-<
  
+--------------------------+
| L5                       |
+==========================+
|                      128 |
|                      128 |
+--------------------------+
```

All of the functions are currently scalars. Next steps include adding BAT functions, including some of the constants at the beginning of [Rmath.h](http://docs.rexamine.com/R-devel/Rmath_8h_source.html), and adding some statistical tests. As examples, I have included: `poisson_ci(y,boundary)` for an exact Poisson confidence interval for count y for a specific boundary (1=left, 2=right) with default confidence level of 95%; and `poisson_test(y,t,r)` for count y, time exposed t and ratio r under the null hypothesis:

``` SQL
select 10.0/9 as rate_ratio, poisson_ci(10,1)/9 as lci, poisson_ci(10,2)/9 as uci, poisson_test(10,9) as pvalue;
  
+------------+--------------------------+--------------------------+--------------------------+
| rate_ratio | lci                      | uci                      | pvalue                   |
+============+==========================+==========================+==========================+
|      1.111 |       0.5328209662369371 |       2.0433728935575304 |       0.7364887199809547 |
+------------+--------------------------+--------------------------+--------------------------+

```

The equivalent R would be:

``` R
> poisson.test(10,9)

	Exact Poisson test

data:  10 time base: 9
number of events = 10, time base = 9, p-value = 0.7365
alternative hypothesis: true event rate is not equal to 1
95 percent confidence interval:
 0.532821 2.043373
sample estimates:
event rate 
  1.111111 
```

