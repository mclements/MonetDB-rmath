/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.  If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 1997 - July 2008 CWI, August 2008 - 2017 MonetDB B.V.
 */

/*
 *  A. de Rijke, B. Scheers
 * The gsl module
 * The gsl module contains wrappers for functions in
 * gsl.
 */

#include "monetdb_config.h"
#include "mal.h"
#include "mal_exception.h"

#define MATHLIB_STANDALONE
#include "Rmath.h"

#ifdef WIN32
#define rmath_export extern __declspec(dllexport)
#else
#define rmath_export extern
#endif


#define DIST2FUNCTION(NAME,ARG1,ARG2)					\
  static str								\
  rmathp ## NAME(dbl *retval, dbl q, dbl ARG1, dbl ARG2, int lower, int log)	\
  {									\
    *retval = dbl_nil;							\
    if (q == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for q" );			\
    if (ARG1 == dbl_nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG1);		\
    if (ARG2 == dbl_nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG2);		\
    if (lower == dbl_nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for lower");		\
    if (log == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for log");		\
    *retval = p ## NAME(q, ARG1, ARG2, lower, log);			\
    return MAL_SUCCEED;							\
  }									\
  static str								\
  rmathq ## NAME(dbl *retval, dbl p, dbl ARG1, dbl ARG2, int lower, int log)	\
  {									\
    *retval = dbl_nil;							\
    if (p == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for p" );			\
    if (ARG1 == dbl_nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG1);		\
    if (ARG2 == dbl_nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG2);		\
    if (lower == dbl_nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for lower");		\
    if (log == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for log");		\
    *retval = q ## NAME(p, ARG1, ARG2, lower, log);			\
    return MAL_SUCCEED;							\
  }									\
  static str								\
  rmathd ## NAME(dbl *retval, dbl x, dbl ARG1, dbl ARG2, int log)	\
  {									\
    *retval = dbl_nil;							\
    if (x == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for x" );			\
    if (ARG1 == dbl_nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG1);		\
    if (ARG2 == dbl_nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG2);		\
    if (log == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for log");		\
    *retval = d ## NAME(x, ARG1, ARG2, log);				\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  rmath_p ## NAME(dbl *retval, dbl *q, dbl *ARG1, dbl *ARG2, int *lower, int *log) \
  {									\
    return rmathp ## NAME(retval, *q, *ARG1, *ARG2, *lower, *log);	\
  }									\
  rmath_export str							\
  rmath_q ## NAME(dbl *retval, dbl *p, dbl *ARG1, dbl *ARG2, int *lower, int *log) \
  {									\
    return rmathq ## NAME(retval, *p, *ARG1, *ARG2, *lower, *log);	\
  }									\
  rmath_export str							\
  rmath_d ## NAME(dbl *retval, dbl *x, dbl *ARG1, dbl *ARG2, int *log) \
  {									\
    return rmathd ## NAME(retval, *x, *ARG1, *ARG2, *log);		\
  }
DIST2FUNCTION(norm,mean,sd)
DIST2FUNCTION(gamma,shape,scale)

#define DIST1FUNCTION(NAME,ARG)						\
  static str								\
  rmathp ## NAME(dbl *retval, dbl q, dbl ARG, int lower, int log)	\
  {									\
    *retval = dbl_nil;							\
    if (q == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for q" );			\
    if (ARG == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG);		\
    if (lower == dbl_nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for lower");		\
    if (log == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for log");		\
    *retval = p ## NAME(q, ARG, lower, log);				\
    return MAL_SUCCEED;							\
  }									\
  static str								\
  rmathq ## NAME(dbl *retval, dbl p, dbl ARG, int lower, int log)	\
  {									\
    *retval = dbl_nil;							\
    if (p == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for p" );			\
    if (ARG == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG);		\
    if (lower == dbl_nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for lower");		\
    if (log == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for log");		\
    *retval = q ## NAME(p, ARG, lower, log);				\
    return MAL_SUCCEED;							\
  }									\
  static str								\
  rmathd ## NAME(dbl *retval, dbl x, dbl ARG, int log)			\
  {									\
    *retval = dbl_nil;							\
    if (x == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for x" );			\
    if (ARG == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG);		\
    if (log == dbl_nil)							\
      throw(MAL, "rmath." #NAME, "Wrong value for log");		\
    *retval = d ## NAME(x, ARG, log);					\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  rmath_p ## NAME(dbl *retval, dbl *q, dbl *ARG, int *lower, int *log)	\
  {									\
    return rmathp ## NAME(retval, *q, *ARG, *lower, *log);		\
  }									\
  rmath_export str							\
  rmath_q ## NAME(dbl *retval, dbl *p, dbl *ARG, int *lower, int *log)	\
  {									\
    return rmathq ## NAME(retval, *p, *ARG, *lower, *log);		\
  }									\
  rmath_export str							\
  rmath_d ## NAME(dbl *retval, dbl *x, dbl *ARG, int *log)		\
  {									\
    return rmathd ## NAME(retval, *x, *ARG, *log);			\
  }
DIST1FUNCTION(chisq,df)

