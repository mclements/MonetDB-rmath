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

#define CONSTANTFUN(LABEL,NAME)						\
  static str								\
  rmath ## NAME(dbl *retval)						\
  {									\
    *retval = dbl_nil;							\
    *retval = NAME;							\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  rmath_ ## NAME(dbl *retval)						\
  {									\
    return rmath ## NAME(retval);					\
  }

#define FUNCTION0(NAME)							\
  static str								\
  rmath ## NAME(dbl *retval)						\
  {									\
    *retval = dbl_nil;							\
    *retval = NAME();							\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  rmath_ ## NAME(dbl *retval)						\
  {									\
    return rmath ## NAME(retval);					\
  }

#define FUNCTION1(NAME,TYPE,ARG)					\
  static str								\
  rmath ## NAME(dbl *retval, TYPE ARG)					\
  {									\
    *retval = dbl_nil;							\
    if (ARG == TYPE ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG);		\
    *retval = NAME(ARG);						\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  rmath_ ## NAME(dbl *retval, TYPE *ARG)				\
  {									\
    return rmath ## NAME(retval, *ARG);					\
  }									\
  static str								\
  batrmath ## NAME(bat *retval, bat ARG)				\
  {									\
    BAT *b_, *bn;							\
    BATiter bi;								\
    BUN p_, q_;								\
    dbl r_;								\
    if ((b_ = BATdescriptor(ARG)) == NULL) {				\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    bi = bat_iterator(b_);						\
    bn = COLnew(b_->hseqbase, TYPE_dbl, BATcount(b_), TRANSIENT);	\
    if (bn == NULL) {							\
      BBPunfix(b_->batCacheid);						\
      throw(MAL, "batrmath." # NAME, MAL_MALLOC_FAIL);			\
    }									\
    BATloop(b_, p_, q_) {						\
      dbl d_ = *(dbl *) BUNtail(bi, p_);				\
      if (d_ == dbl_nil) {						\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for " # ARG);	\
      }									\
      r_ = NAME(d_);							\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    *retval = bn->batCacheid;						\
    BBPkeepref(bn->batCacheid);						\
    BBPunfix(b_->batCacheid);						\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  bat_rmath_ ## NAME(bat *retval, bat *ARG)				\
  {									\
    return batrmath ## NAME(retval, *ARG);				\
  }

#define FUNCTION2(NAME,TYPE1,ARG1,TYPE2,ARG2)				\
  static str								\
  rmath ## NAME(dbl *retval, TYPE1 ARG1, TYPE2 ARG2)			\
  {									\
    *retval = dbl_nil;							\
    if (ARG1 == TYPE1 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG1);		\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG2);		\
    *retval = NAME(ARG1,ARG2);						\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  rmath_ ## NAME(dbl *retval, TYPE1 *ARG1, TYPE2 *ARG2)			\
  {									\
    return rmath ## NAME(retval, *ARG1, *ARG2);				\
  }									\
  static str								\
  batrmath ## NAME(bat *retval, bat ARG1, TYPE2 ARG2)			\
  {									\
    BAT *b_, *bn;							\
    BATiter bi;								\
    BUN p_, q_;								\
    dbl r_;								\
    if ((b_ = BATdescriptor(ARG1)) == NULL) {				\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG2);		\
    bi = bat_iterator(b_);						\
    bn = COLnew(b_->hseqbase, TYPE_dbl, BATcount(b_), TRANSIENT);	\
    if (bn == NULL) {							\
      BBPunfix(b_->batCacheid);						\
      throw(MAL, "batrmath." # NAME, MAL_MALLOC_FAIL);			\
    }									\
    BATloop(b_, p_, q_) {						\
      dbl d_ = *(dbl *) BUNtail(bi, p_);				\
      if (d_ == dbl_nil) {						\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for " # ARG1);	\
      }									\
      r_ = NAME(d_,ARG2);						\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    *retval = bn->batCacheid;						\
    BBPkeepref(bn->batCacheid);						\
    BBPunfix(b_->batCacheid);						\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
    bat_rmath_ ## NAME(bat *retval, bat *ARG1, TYPE2 *ARG2)		\
  {									\
    return batrmath ## NAME(retval, *ARG1, *ARG2);			\
  }									\
  static str								\
  batrmathbats ## NAME(bat *retval, bat ARG1, bat ARG2)			\
  {									\
    BAT * arg[2];							\
    BAT *bn;								\
    dbl r_;								\
    TYPE1* valp1;							\
    TYPE2* valp2;							\
    size_t cnt = 0, i, j, kk, ncol=2;					\
    if (((arg[j=0] = BATdescriptor(ARG1)) == NULL) ||			\
	((arg[j=1] = BATdescriptor(ARG2)) == NULL)) {			\
      for (kk = 0; kk < j; kk++)					\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    bn = COLnew(arg[0]->hseqbase, TYPE_dbl, cnt = BATcount(arg[0]), TRANSIENT); \
    if (bn == NULL) {							\
      for (kk=0; kk<ncol; kk++)						\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    valp1 = (TYPE1 *) Tloc(arg[0], 0);					\
    valp2 = (TYPE2 *) Tloc(arg[1], 0);					\
    for (i = 0; i < cnt; i++) {						\
      if (valp1[i] == TYPE1 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG1); \
      }									\
      if (valp2[i] == TYPE2 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG2); \
      }									\
      r_ = NAME(valp1[i], valp2[i]);					\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    BBPkeepref(*retval = bn->batCacheid);				\
    for (kk=0; kk<ncol; kk++)						\
      BBPunfix(arg[kk]->batCacheid);					\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  bat_rmath_bats_ ## NAME(bat *retval, bat *ARG1, bat *ARG2)		\
  {									\
    return batrmathbats ## NAME(retval, *ARG1, *ARG2);			\
  }									
  
#define FUNCTION3(NAME,TYPE1,ARG1,TYPE2,ARG2,TYPE3,ARG3)		\
  static str								\
  rmath ## NAME(dbl *retval, TYPE1 ARG1, TYPE2 ARG2, TYPE3 ARG3)	\
  {									\
    *retval = dbl_nil;							\
    if (ARG1 == TYPE1 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG1);		\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG3);		\
    *retval = NAME(ARG1,ARG2,ARG3);					\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  rmath_ ## NAME(dbl *retval, TYPE1 *ARG1, TYPE2 *ARG2, TYPE3 *ARG3)	\
  {									\
    return rmath ## NAME(retval, *ARG1, *ARG2, *ARG3);			\
  }									\
  static str								\
  batrmath ## NAME(bat *retval, bat ARG1, TYPE2 ARG2, TYPE3 ARG3)	\
  {									\
    BAT *b_, *bn;							\
    BATiter bi;								\
    BUN p_, q_;								\
    dbl r_;								\
    if ((b_ = BATdescriptor(ARG1)) == NULL) {				\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG3);		\
    bi = bat_iterator(b_);						\
    bn = COLnew(b_->hseqbase, TYPE_dbl, BATcount(b_), TRANSIENT);	\
    if (bn == NULL) {							\
      BBPunfix(b_->batCacheid);						\
      throw(MAL, "batrmath." # NAME, MAL_MALLOC_FAIL);			\
    }									\
    BATloop(b_, p_, q_) {						\
      dbl d_ = *(dbl *) BUNtail(bi, p_);				\
      if (d_ == dbl_nil) {						\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for " # ARG1);	\
      }									\
      r_ = NAME(d_,ARG2,ARG3);						\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    *retval = bn->batCacheid;						\
    BBPkeepref(bn->batCacheid);						\
    BBPunfix(b_->batCacheid);						\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  bat_rmath_ ## NAME(bat *retval, bat *ARG1, TYPE2 *ARG2, TYPE3 *ARG3)	\
  {									\
    return batrmath ## NAME(retval, *ARG1, *ARG2, *ARG3);		\
  }									\
  static str								\
  batrmathbats ## NAME(bat *retval, bat ARG1, bat ARG2, bat ARG3)	\
  {									\
    BAT * arg[3];							\
    BAT *bn;								\
    dbl r_;								\
    TYPE1* valp1;							\
    TYPE2* valp2;							\
    TYPE3* valp3;							\
    size_t cnt = 0, i, j, kk, ncol=3;					\
    if (((arg[j=0] = BATdescriptor(ARG1)) == NULL) ||			\
	((arg[j=1] = BATdescriptor(ARG2)) == NULL) ||			\
	((arg[j=2] = BATdescriptor(ARG3)) == NULL)) {			\
      for (kk = 0; kk < j; kk++)					\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    bn = COLnew(arg[0]->hseqbase, TYPE_dbl, cnt = BATcount(arg[0]), TRANSIENT); \
    if (bn == NULL) {							\
      for (kk=0; kk<ncol; kk++)						\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    valp1 = (TYPE1 *) Tloc(arg[0], 0);					\
    valp2 = (TYPE2 *) Tloc(arg[1], 0);					\
    valp3 = (TYPE3 *) Tloc(arg[2], 0);					\
    for (i = 0; i < cnt; i++) {						\
      if (valp1[i] == TYPE1 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG1); \
      }									\
      if (valp2[i] == TYPE2 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG2); \
      }									\
      if (valp3[i] == TYPE3 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG3); \
      }									\
      r_ = NAME(valp1[i], valp2[i], valp3[i]);				\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    BBPkeepref(*retval = bn->batCacheid);				\
    for (kk=0; kk<ncol; kk++)						\
      BBPunfix(arg[kk]->batCacheid);					\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  bat_rmath_bats_ ## NAME(bat *retval, bat *ARG1, bat *ARG2, bat *ARG3) \
  {									\
    return batrmathbats ## NAME(retval, *ARG1, *ARG2, *ARG3);		\
  }									
  
#define FUNCTION4(NAME,TYPE1,ARG1,TYPE2,ARG2,TYPE3,ARG3,TYPE4,ARG4)	\
  static str								\
  rmath ## NAME(dbl *retval, TYPE1 ARG1, TYPE2 ARG2, TYPE3 ARG3, TYPE4 ARG4) \
  {									\
    *retval = dbl_nil;							\
    if (ARG1 == TYPE1 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG1);		\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG3);		\
    if (ARG4 == TYPE4 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG4);		\
    *retval = NAME(ARG1,ARG2,ARG3,ARG4);				\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  rmath_ ## NAME(dbl *retval, TYPE1 *ARG1, TYPE2 *ARG2, TYPE3 *ARG3, TYPE4 *ARG4) \
  {									\
    return rmath ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4);		\
  }									\
  static str								\
  batrmath ## NAME(bat *retval, bat ARG1, TYPE2 ARG2, TYPE3 ARG3, TYPE4 ARG4) \
  {									\
    BAT *b_, *bn;							\
    BATiter bi;								\
    BUN p_, q_;								\
    dbl r_;								\
    if ((b_ = BATdescriptor(ARG1)) == NULL) {				\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG3);		\
    if (ARG4 == TYPE4 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG4);		\
    bi = bat_iterator(b_);						\
    bn = COLnew(b_->hseqbase, TYPE_dbl, BATcount(b_), TRANSIENT);	\
    if (bn == NULL) {							\
      BBPunfix(b_->batCacheid);						\
      throw(MAL, "batrmath." # NAME, MAL_MALLOC_FAIL);			\
    }									\
    BATloop(b_, p_, q_) {						\
      dbl d_ = *(dbl *) BUNtail(bi, p_);				\
      if (d_ == dbl_nil) {						\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for " # ARG1);	\
      }									\
      r_ = NAME(d_,ARG2,ARG3,ARG4);					\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    *retval = bn->batCacheid;						\
    BBPkeepref(bn->batCacheid);						\
    BBPunfix(b_->batCacheid);						\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  bat_rmath_ ## NAME(bat *retval, bat *ARG1, TYPE2 *ARG2, TYPE3 *ARG3, TYPE4 *ARG4) \
  {									\
    return batrmath ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4);	\
  }									\
  static str								\
  batrmathbats ## NAME(bat *retval, bat ARG1, bat ARG2, bat ARG3, bat ARG4) \
  {									\
    BAT * arg[4];							\
    BAT *bn;								\
    dbl r_;								\
    TYPE1* valp1;							\
    TYPE2* valp2;							\
    TYPE3* valp3;							\
    TYPE4* valp4;							\
    size_t cnt = 0, i, j, kk, ncol=4;					\
    if (((arg[j=0] = BATdescriptor(ARG1)) == NULL) ||			\
	((arg[j=1] = BATdescriptor(ARG2)) == NULL) ||			\
	((arg[j=2] = BATdescriptor(ARG3)) == NULL) ||			\
	((arg[j=3] = BATdescriptor(ARG4)) == NULL)) {			\
      for (kk = 0; kk < j; kk++)					\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    bn = COLnew(arg[0]->hseqbase, TYPE_dbl, cnt = BATcount(arg[0]), TRANSIENT); \
    if (bn == NULL) {							\
      for (kk=0; kk<ncol; kk++)						\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    valp1 = (TYPE1 *) Tloc(arg[0], 0);					\
    valp2 = (TYPE2 *) Tloc(arg[1], 0);					\
    valp3 = (TYPE3 *) Tloc(arg[2], 0);					\
    valp4 = (TYPE4 *) Tloc(arg[3], 0);					\
    for (i = 0; i < cnt; i++) {						\
      if (valp1[i] == TYPE1 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG1); \
      }									\
      if (valp2[i] == TYPE2 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG2); \
      }									\
      if (valp3[i] == TYPE3 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG3); \
      }									\
      if (valp4[i] == TYPE4 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG4); \
      }									\
      r_ = NAME(valp1[i], valp2[i], valp3[i], valp4[i]); \
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    BBPkeepref(*retval = bn->batCacheid);				\
    for (kk=0; kk<ncol; kk++)						\
      BBPunfix(arg[kk]->batCacheid);					\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  bat_rmath_bats_ ## NAME(bat *retval, bat *ARG1, bat *ARG2, bat *ARG3, bat *ARG4) \
  {									\
    return batrmathbats ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4); \
  }									

#define FUNCTION5(NAME,TYPE1,ARG1,TYPE2,ARG2,TYPE3,ARG3,TYPE4,ARG4,TYPE5,ARG5) \
  static str								\
  rmath ## NAME(dbl *retval, TYPE1 ARG1, TYPE2 ARG2, TYPE3 ARG3, TYPE4 ARG4, TYPE5 ARG5) \
  {									\
    *retval = dbl_nil;							\
    if (ARG1 == TYPE1 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG1);		\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG3);		\
    if (ARG4 == TYPE4 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG4);		\
    if (ARG5 == TYPE5 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG5);		\
    *retval = NAME(ARG1,ARG2,ARG3,ARG4,ARG5);				\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  rmath_ ## NAME(dbl *retval, TYPE1 *ARG1, TYPE2 *ARG2, TYPE3 *ARG3, TYPE4 *ARG4, TYPE5 *ARG5) \
  {									\
    return rmath ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4, *ARG5);	\
  }									\
  static str								\
  batrmath ## NAME(bat *retval, bat ARG1, TYPE2 ARG2, TYPE3 ARG3, TYPE4 ARG4, TYPE4 ARG5) \
  {									\
    BAT *b_, *bn;							\
    BATiter bi;								\
    BUN p_, q_;								\
    dbl r_;								\
    if ((b_ = BATdescriptor(ARG1)) == NULL) {				\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG3);		\
    if (ARG4 == TYPE4 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG4);		\
    if (ARG5 == TYPE5 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG5);		\
    bi = bat_iterator(b_);						\
    bn = COLnew(b_->hseqbase, TYPE_dbl, BATcount(b_), TRANSIENT);	\
    if (bn == NULL) {							\
      BBPunfix(b_->batCacheid);						\
      throw(MAL, "batrmath." # NAME, MAL_MALLOC_FAIL);			\
    }									\
    BATloop(b_, p_, q_) {						\
      dbl d_ = *(dbl *) BUNtail(bi, p_);				\
      if (d_ == dbl_nil) {						\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for " # ARG1);	\
      }									\
      r_ = NAME(d_,ARG2,ARG3,ARG4,ARG5);				\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    *retval = bn->batCacheid;						\
    BBPkeepref(bn->batCacheid);						\
    BBPunfix(b_->batCacheid);						\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
    bat_rmath_ ## NAME(bat *retval, bat *ARG1, TYPE2 *ARG2, TYPE3 *ARG3, TYPE4 *ARG4, TYPE5 *ARG5) \
  {									\
    return batrmath ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4, *ARG5); \
  }									\
  static str								\
  batrmathbats ## NAME(bat *retval, bat ARG1, bat ARG2, bat ARG3, bat ARG4, bat ARG5) \
  {									\
    BAT * arg[5];							\
    BAT *bn;								\
    dbl r_;								\
    TYPE1* valp1;							\
    TYPE2* valp2;							\
    TYPE3* valp3;							\
    TYPE4* valp4;							\
    TYPE5* valp5;							\
    size_t cnt = 0, i, j, kk, ncol=5;					\
    if (((arg[j=0] = BATdescriptor(ARG1)) == NULL) ||			\
	((arg[j=1] = BATdescriptor(ARG2)) == NULL) ||			\
	((arg[j=2] = BATdescriptor(ARG3)) == NULL) ||			\
	((arg[j=3] = BATdescriptor(ARG4)) == NULL) ||			\
	((arg[j=4] = BATdescriptor(ARG5)) == NULL)) {			\
      for (kk = 0; kk < j; kk++)					\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    bn = COLnew(arg[0]->hseqbase, TYPE_dbl, cnt = BATcount(arg[0]), TRANSIENT); \
    if (bn == NULL) {							\
      for (kk=0; kk<ncol; kk++)						\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    valp1 = (TYPE1 *) Tloc(arg[0], 0);					\
    valp2 = (TYPE2 *) Tloc(arg[1], 0);					\
    valp3 = (TYPE3 *) Tloc(arg[2], 0);					\
    valp4 = (TYPE4 *) Tloc(arg[3], 0);					\
    valp5 = (TYPE5 *) Tloc(arg[4], 0);					\
    for (i = 0; i < cnt; i++) {						\
      if (valp1[i] == TYPE1 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG1); \
      }									\
      if (valp2[i] == TYPE2 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG2); \
      }									\
      if (valp3[i] == TYPE3 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG3); \
      }									\
      if (valp4[i] == TYPE4 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG4); \
      }									\
      if (valp5[i] == TYPE5 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG5); \
      }									\
      r_ = NAME(valp1[i], valp2[i], valp3[i], valp4[i], valp5[i]); \
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    BBPkeepref(*retval = bn->batCacheid);				\
    for (kk=0; kk<ncol; kk++)						\
      BBPunfix(arg[kk]->batCacheid);					\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  bat_rmath_bats_ ## NAME(bat *retval, bat *ARG1, bat *ARG2, bat *ARG3, bat *ARG4, bat *ARG5) \
  {									\
    return batrmathbats ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4, *ARG5); \
  }									

#define FUNCTION6(NAME,TYPE1,ARG1,TYPE2,ARG2,TYPE3,ARG3,TYPE4,ARG4,TYPE5,ARG5,TYPE6,ARG6) \
  static str								\
  rmath ## NAME(dbl *retval, TYPE1 ARG1, TYPE2 ARG2, TYPE3 ARG3, TYPE4 ARG4, TYPE5 ARG5, TYPE6 ARG6) \
  {									\
    *retval = dbl_nil;							\
    if (ARG1 == TYPE1 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG1);		\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG3);		\
    if (ARG4 == TYPE4 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG4);		\
    if (ARG5 == TYPE5 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG5);		\
    if (ARG6 == TYPE6 ## _nil)						\
      throw(MAL, "rmath." #NAME, "Wrong value for " #ARG6);		\
    *retval = NAME(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6);			\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  rmath_ ## NAME(dbl *retval, TYPE1 *ARG1, TYPE2 *ARG2, TYPE3 *ARG3, TYPE4 *ARG4, TYPE5 *ARG5, TYPE6 *ARG6) \
  {									\
    return rmath ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4, *ARG5, *ARG6); \
  }									\
  static str								\
  batrmath ## NAME(bat *retval, bat ARG1, TYPE2 ARG2, TYPE3 ARG3, TYPE4 ARG4, TYPE5 ARG5, TYPE6 ARG6) \
  {									\
    BAT *b_, *bn;							\
    BATiter bi;								\
    BUN p_, q_;								\
    dbl r_;								\
    if ((b_ = BATdescriptor(ARG1)) == NULL) {				\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    if (ARG2 == TYPE2 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG2);		\
    if (ARG3 == TYPE3 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG3);		\
    if (ARG4 == TYPE4 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG4);		\
    if (ARG5 == TYPE5 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG5);		\
    if (ARG6 == TYPE6 ## _nil)						\
      throw(MAL, "batrmath." #NAME, "Wrong value for " #ARG6);		\
    bi = bat_iterator(b_);						\
    bn = COLnew(b_->hseqbase, TYPE_dbl, BATcount(b_), TRANSIENT);	\
    if (bn == NULL) {							\
      BBPunfix(b_->batCacheid);						\
      throw(MAL, "batrmath." # NAME, MAL_MALLOC_FAIL);			\
    }									\
    BATloop(b_, p_, q_) {						\
      dbl d_ = *(dbl *) BUNtail(bi, p_);				\
      if (d_ == dbl_nil) {						\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for " # ARG1);	\
      }									\
      r_ = NAME(d_,ARG2,ARG3,ARG4,ARG5,ARG6);				\
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	BBPunfix(b_->batCacheid);					\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    *retval = bn->batCacheid;						\
    BBPkeepref(bn->batCacheid);						\
    BBPunfix(b_->batCacheid);						\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
    bat_rmath_ ## NAME(bat *retval, bat *ARG1, TYPE2 *ARG2, TYPE3 *ARG3, TYPE4 *ARG4, TYPE5 *ARG5, TYPE6 *ARG6) \
  {									\
    return batrmath ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4, *ARG5, *ARG6); \
  }									\
  static str								\
  batrmathbats ## NAME(bat *retval, bat ARG1, bat ARG2, bat ARG3, bat ARG4, bat ARG5, bat ARG6) \
  {									\
    BAT * arg[6];							\
    BAT *bn;								\
    dbl r_;								\
    TYPE1* valp1;							\
    TYPE2* valp2;							\
    TYPE3* valp3;							\
    TYPE4* valp4;							\
    TYPE5* valp5;							\
    TYPE6* valp6;							\
    size_t cnt = 0, i, j, kk, ncol=6;					\
    if (((arg[j=0] = BATdescriptor(ARG1)) == NULL) ||			\
	((arg[j=1] = BATdescriptor(ARG2)) == NULL) ||			\
	((arg[j=2] = BATdescriptor(ARG3)) == NULL) ||			\
	((arg[j=3] = BATdescriptor(ARG4)) == NULL) ||			\
	((arg[j=4] = BATdescriptor(ARG5)) == NULL) ||			\
	((arg[j=5] = BATdescriptor(ARG6)) == NULL)) {			\
      for (kk = 0; kk < j; kk++)					\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    bn = COLnew(arg[0]->hseqbase, TYPE_dbl, cnt = BATcount(arg[0]), TRANSIENT); \
    if (bn == NULL) {							\
      for (kk=0; kk<ncol; kk++)						\
	BBPunfix(arg[kk]->batCacheid);					\
      throw(MAL, "batrmath." # NAME, RUNTIME_OBJECT_MISSING);		\
    }									\
    valp1 = (TYPE1 *) Tloc(arg[0], 0);					\
    valp2 = (TYPE2 *) Tloc(arg[1], 0);					\
    valp3 = (TYPE3 *) Tloc(arg[2], 0);					\
    valp4 = (TYPE4 *) Tloc(arg[3], 0);					\
    valp5 = (TYPE5 *) Tloc(arg[4], 0);					\
    valp6 = (TYPE6 *) Tloc(arg[5], 0);					\
    for (i = 0; i < cnt; i++) {						\
      if (valp1[i] == TYPE1 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG1); \
      }									\
      if (valp2[i] == TYPE2 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG2); \
      }									\
      if (valp3[i] == TYPE3 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG3); \
      }									\
      if (valp4[i] == TYPE4 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG4); \
      }									\
      if (valp5[i] == TYPE5 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG5); \
      }									\
      if (valp6[i] == TYPE6 ## _nil) {					\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, "Wrong value for argument" # ARG6); \
      }									\
      r_ = NAME(valp1[i], valp2[i], valp3[i], valp4[i], valp5[i], valp6[i]); \
      if (BUNappend(bn, &r_, FALSE) != GDK_SUCCEED) {			\
	for (kk = 0; kk < ncol; kk++)					\
	  BBPunfix(arg[kk]->batCacheid);				\
	BBPreclaim(bn);							\
	throw(MAL, "batrmath." # NAME, GDK_EXCEPTION);			\
      }									\
    }									\
    BBPkeepref(*retval = bn->batCacheid);				\
    for (kk=0; kk<ncol; kk++)						\
      BBPunfix(arg[kk]->batCacheid);					\
    return MAL_SUCCEED;							\
  }									\
  rmath_export str							\
  bat_rmath_bats_ ## NAME(bat *retval, bat *ARG1, bat *ARG2, bat *ARG3, bat *ARG4, bat *ARG5, bat *ARG6) \
  {									\
    return batrmathbats ## NAME(retval, *ARG1, *ARG2, *ARG3, *ARG4, *ARG5, *ARG6); \
  }									

#define DIST1FUNCTION(NAME,ARG)						\
  FUNCTION4(p ## NAME,dbl,q,dbl,ARG,int,lower,int,log)			\
  FUNCTION4(q ## NAME,dbl,p,dbl,ARG,int,lower,int,log)			\
  FUNCTION3(d ## NAME,dbl,x,dbl,ARG,int,log)				\
  FUNCTION1(r ## NAME,dbl,ARG)				
  
#define DIST2FUNCTION(NAME,ARG1,ARG2)					\
  FUNCTION5(p ## NAME,dbl,q,dbl,ARG1,dbl,ARG2,int,lower,int,log)	\
  FUNCTION5(q ## NAME,dbl,p,dbl,ARG1,dbl,ARG2,int,lower,int,log)	\
  FUNCTION4(d ## NAME,dbl,x,dbl,ARG1,dbl,ARG2,int,log)			\
  FUNCTION2(r ## NAME,dbl,ARG1,dbl,ARG2)				
#define DIST3FUNCTION(NAME,ARG1,ARG2,ARG3)				\
  FUNCTION6(p ## NAME,dbl,q,dbl,ARG1,dbl,ARG2,dbl,ARG3,int,lower,int,log) \
  FUNCTION6(q ## NAME,dbl,p,dbl,ARG1,dbl,ARG2,dbl,ARG3,int,lower,int,log) \
  FUNCTION5(d ## NAME,dbl,x,dbl,ARG1,dbl,ARG2,dbl,ARG3,int,log)		\
  FUNCTION3(r ## NAME,dbl,ARG1,dbl,ARG2,dbl,ARG3)				

CONSTANTFUN(e,M_E)
CONSTANTFUN(log2(e),M_LOG2E)
CONSTANTFUN(log10(e),M_LOG10E)
CONSTANTFUN(ln(2),M_LN2)
CONSTANTFUN(ln(10),M_LN10)
CONSTANTFUN(pi,M_PI)
CONSTANTFUN(2*pi,M_2PI)
CONSTANTFUN(pi/2,M_PI_2)
CONSTANTFUN(pi/4,M_PI_4)
CONSTANTFUN(1/pi,M_1_PI)
CONSTANTFUN(2/pi,M_2_PI)
CONSTANTFUN(2/sqrt(pi),M_2_SQRTPI)
CONSTANTFUN(sqrt(2),M_SQRT2)
CONSTANTFUN(1/sqrt(2),M_SQRT1_2)
CONSTANTFUN(sqrt(3),M_SQRT_3)
CONSTANTFUN(sqrt(32),M_SQRT_32)
CONSTANTFUN(log10(2),M_LOG10_2)
CONSTANTFUN(sqrt(pi),M_SQRT_PI)
CONSTANTFUN(1/sqrt(2pi),M_1_SQRT_2PI)
CONSTANTFUN(sqrt(2/pi),M_SQRT_2dPI)
CONSTANTFUN(log(2*pi),M_LN_2PI)
CONSTANTFUN(log(pi)/2,M_LN_SQRT_PI)
CONSTANTFUN(log(2*pi)/2,M_LN_SQRT_2PI)
CONSTANTFUN(log(pi/2)/2,M_LN_SQRT_PId2)
FUNCTION2(R_pow,dbl,x,dbl,y)
FUNCTION2(R_pow_di,dbl,x,int,y)
FUNCTION0(norm_rand)
FUNCTION0(unif_rand)
FUNCTION0(exp_rand)
static str
rmathset_seed(dbl *retval, int a, int b)
{
  *retval = dbl_nil;
  set_seed((unsigned int) a, (unsigned int) b);
  *retval = 1.0;
  return MAL_SUCCEED;
}				
rmath_export str		
rmath_set_seed(dbl *retval, int *a, int *b)	
{				
  return rmathset_seed(retval,*a,*b);
}
// void	get_seed(unsigned int *, unsigned int *);
DIST2FUNCTION(norm,mean,sd)
DIST2FUNCTION(unif,min,max)
DIST2FUNCTION(gamma,shape,scale)
FUNCTION1(log1pmx,dbl,x)
FUNCTION1(log1pexp,dbl,x)
FUNCTION1(lgamma1p,dbl,x)
FUNCTION2(logspace_add,dbl,logx,dbl,logy)
FUNCTION2(logspace_sub,dbl,logx,dbl,logy)
// double  logspace_sum(const double *, int);
DIST2FUNCTION(beta,shape1,shape2)
DIST2FUNCTION(lnorm,meanlog,sdlog)
DIST1FUNCTION(chisq,df)
DIST2FUNCTION(nchisq,df,ncp)
DIST2FUNCTION(f,df1,df2)
DIST1FUNCTION(t,df)
DIST2FUNCTION(binom,size,prob)
// void	rmultinom(int, double*, int, int*);
DIST2FUNCTION(cauchy,location,scale)
DIST1FUNCTION(exp,rate)
DIST1FUNCTION(geom,prob)
DIST3FUNCTION(hyper,m,n,k)
DIST2FUNCTION(nbinom,size,prob)
DIST1FUNCTION(pois,lambda)
DIST2FUNCTION(weibull,shape,scale)
DIST2FUNCTION(logis,location,scale)
// DIST3FUNCTION(nbeta,shape1,shape2,ncp)
FUNCTION6(pnbeta,dbl,q,dbl,shape1,dbl,shape2,dbl,ncp,int,lower,int,log)
FUNCTION6(qnbeta,dbl,p,dbl,shape1,dbl,shape2,dbl,ncp,int,lower,int,log)
FUNCTION5(dnbeta,dbl,x,dbl,shape1,dbl,shape2,dbl,ncp,int,log)
// FUNCTION3(rnbeta,dbl,shape1,dbl,shape2,dbl,ncp)
// DIST3FUNCTION(nf,df1,df2,ncp)
FUNCTION6(pnf,dbl,q,dbl,df1,dbl,df2,dbl,ncp,int,lower,int,log)
FUNCTION6(qnf,dbl,p,dbl,df1,dbl,df2,dbl,ncp,int,lower,int,log)
FUNCTION5(dnf,dbl,x,dbl,df1,dbl,df2,dbl,ncp,int,log)
//DIST2FUNCTION(nt,df,ncp)
FUNCTION5(pnt,dbl,q,dbl,df,dbl,ncp,int,lower,int,log)
FUNCTION5(qnt,dbl,p,dbl,df,dbl,ncp,int,lower,int,log)
FUNCTION4(dnt,dbl,x,dbl,df,dbl,ncp,int,log)
FUNCTION6(ptukey,dbl,q,dbl,nmeans,dbl,df,dbl,nranges,int,lower,int,log)
FUNCTION6(qtukey,dbl,p,dbl,nmeans,dbl,df,dbl,nranges,int,lower,int,log)
DIST2FUNCTION(wilcox, m, n)
DIST1FUNCTION(signrank,n)
FUNCTION1(gammafn,dbl,x)
FUNCTION1(lgammafn,dbl,x)
// double  lgammafn_sign(double, int*);
// void    dpsifn(double, int, int, int, double*, int*, int*);
FUNCTION2(psigamma,dbl,x,dbl,y)
FUNCTION1(digamma,dbl,x)
FUNCTION1(trigamma,dbl,x)
FUNCTION1(tetragamma,dbl,x)
FUNCTION1(pentagamma,dbl,x)
FUNCTION2(beta,dbl,x,dbl,y)
FUNCTION2(lbeta,dbl,x,dbl,y)
FUNCTION2(choose,dbl,n,dbl,k)
FUNCTION2(lchoose,dbl,n,dbl,k)
FUNCTION3(bessel_i,dbl,x,dbl,nu,dbl,scaled)
FUNCTION2(bessel_j,dbl,x,dbl,nu)
FUNCTION3(bessel_k,dbl,x,dbl,nu,dbl,scaled)
FUNCTION2(bessel_y,dbl,x,dbl,nu)
// double	bessel_i_ex(double, double, double, double *);
// double	bessel_j_ex(double, double, double *);
// double	bessel_k_ex(double, double, double, double *);
// double	bessel_y_ex(double, double, double *);
// FUNCTION2(pythag,dbl,x,dbl,y)
// int	imax2(int, int);
// int	imin2(int, int);
FUNCTION2(fmax2,dbl,x,dbl,y)
FUNCTION2(fmin2,dbl,x,dbl,y)
// FUNCTION1(sign,dbl,x)
FUNCTION2(fprec,dbl,x,dbl,y)
FUNCTION2(fround,dbl,x,dbl,y)
FUNCTION2(fsign,dbl,x,dbl,y)
FUNCTION1(ftrunc,dbl,x)
// FUNCTION1(log1pmx,dbl,x) // repeated
// FUNCTION1(lgamma1p,dbl,x) // repeated
// double cospi(double);
// double sinpi(double);
// double tanpi(double);
// rmath_export dbl NA_REAL;
// rmath_export dbl R_PosInf;
// rmath_export dbl R_NegInf;
// rmath_export int N01_kind;

double rmath_poisson_ci(double x, int boundary, double conflevel) {
  double alpha = (1.0-conflevel)/2.0;
  if (x==0 && boundary==1) return 0.0;
  else if (boundary==1) return qgamma(alpha, x, 1.0, 1, 0);
  else return qgamma(alpha,x + 1,1.0,0,0);
}
FUNCTION3(rmath_poisson_ci,dbl,x,int,boundary,dbl,conflevel)

double rmath_poisson_test(double x, double t, double r, int alternative) {
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
FUNCTION4(rmath_poisson_test,dbl,x,dbl,t,dbl,r,int,alternative)

str
rmath_scalar_bat_poisson_cis(bat *ret, bat *ret2, const dbl *x, const dbl *conflevel)
{
	BAT *a_ = COLnew(0, TYPE_dbl, 1, TRANSIENT);
	BAT *b_ = COLnew(0, TYPE_dbl, 1, TRANSIENT);
	if (a_ == 0 || b_ == 0) {
		if (a_) BBPunfix(a_->batCacheid);
		if (b_) BBPunfix(b_->batCacheid);
		throw(MAL, "rmath.poisson_cis", SQLSTATE(HY001) MAL_MALLOC_FAIL);
	}
	double lci = rmath_poisson_ci(*x, 1, *conflevel);
	double uci = rmath_poisson_ci(*x, 2, *conflevel);
	if (BUNappend(a_, &lci, FALSE) != GDK_SUCCEED ||
		BUNappend(b_, &uci, FALSE) != GDK_SUCCEED)
		goto bailout;
	*ret = a_->batCacheid;
	BBPkeepref(a_->batCacheid);
	*ret2 = b_->batCacheid;
	BBPkeepref(b_->batCacheid);
	return MAL_SUCCEED;
  bailout:
	BBPunfix(a_->batCacheid);
	BBPunfix(b_->batCacheid);
	throw(MAL, "rmath.poisson_cis", SQLSTATE(HY001) MAL_MALLOC_FAIL);
}


// clean up macros
#undef FUNCTION0
#undef FUNCTION1
#undef FUNCTION2
#undef FUNCTION3
#undef FUNCTION4
#undef FUNCTION5
#undef FUNCTION6
#undef DIST1FUNCTION
#undef DIST2FUNCTION
#undef DIST3FUNCTION
#undef rmath_export
#undef MATHLIB_STANDALONE
