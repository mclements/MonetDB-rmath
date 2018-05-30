#ifndef RMATH_EXT_H
#define RMATH_EXT_H

#define MATHLIB_STANDALONE
#include "Rmath.h"

double poisson_ci(double x, int boundary, double conflevel);
double poisson_test(double x, double t, double r, int alternative);

#endif
