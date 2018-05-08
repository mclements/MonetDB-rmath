

# MonetDB-rmath: MonetDB extension to use the Rmath library.

An early version for MonetDB-rmath is now available. It includes approximately 90% of the functions in Rmath.h, including [rpdq](norm|unif|gamma|beta|lnorm|chisq|nchisq|f|t|binom|cauchy|exp|geom|hyper|
nbinom|pois|weibull|logis|wilcox|signrank) and another 40 functions. The current implementation uses m4 for code generation and requires the r-mathlib package under Debian.

One can clone the GitHub repository and install using:

``` SQL
  git clone https://github.com/mclements/MonetDB-rmath.git
  cd MonetDB-rmath
  make
  sudo make install
```


You will then need to use a fresh database. An example script is:

``` SQL
  -- calculate the quantile and cumulative distribution function for a unit normal
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

All of the functions are currently scalars. Next steps include adding BAT functions, including some of the constants at the beginning of Rmath.h, and adding some statistical tests. As an example, I have
included poissonci(y,boundary) for an exact Poisson confidence interval for count y for a specific boundary (1=left, 2=right) with default confidence level of 95%:

``` SQL
  select poissonci(10,1) as lci, poissonci(10,2) as lci;
  
+--------------------------+--------------------------+
| lci                      | lci                      |
+==========================+==========================+
|       4.7953886961324335 |       18.390356042017775 |
+--------------------------+--------------------------+

```

The equivalent R would be

``` R
>  poisson.test(10)

	Exact Poisson test

data:  10 time base: 1
number of events = 10, time base = 1, p-value = 1.114e-07
alternative hypothesis: true event rate is not equal to 1
95 percent confidence interval:
  4.795389 18.390356
sample estimates:
event rate 
        10 
```

