#include "rmath_ext.h"

double poisson_ci(double x, int boundary, double conflevel) {
  double alpha = (1.0-conflevel)/2.0;
  if (x==0 && boundary==1) return 0.0;
  else if (boundary==1) return qgamma(alpha, x, 1.0, 1, 0);
  else return qgamma(1.0-alpha, x + 1,1.0,1,0);
}

double poisson_test(double x, double t, double r, int alternative) {
  int Lower=0, Upper=1; // TwoSided=2;
  double m = r*t;
  if (alternative == Lower) return ppois(x,m,1,0);
  else if (alternative == Upper) return ppois(x-1,m,0,0);
  else {
    if (m==0) return (x==0.0);
    double relErr = 1.0 + 1.0e-7;
    double d = dpois(x,m,0);
    if (x==m) return 1; else if (x<m) {
      int N = ceil(2*m-x);
      while(dpois(N,m,0)>d) N *= 2;
      double y=0.0;
      int i;
      for (i=ceil(m); i<=N; i++) y+=(dpois(i,m,0) <= d *relErr);
      return ppois(x,m,1,0) + ppois(N-y, m,0,0);
    } else {
      double y=0.0;
      int i;
      for (i=0; i<=floor(m); i++) y+=(dpois(i,m,0) <= d *relErr);
      return ppois(y-1,m,1,0) + ppois(x-1, m,0,0);
    }
  }
}

