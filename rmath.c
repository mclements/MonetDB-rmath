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

rmath_export str rmathpchisq(dbl *retval, dbl *chi2, dbl *datapoints);
rmath_export str rmathbat_pchisq_cst(bat *retval, bat *chi2, dbl *datapoints);
rmath_export str rmathcst_pchisq_bat(bat *retval, dbl *chi2, bat *datapoints);
rmath_export str rmathbat_pchisq_bat(bat *retval, bat *chi2, bat *datapoints);

static str
rmath_pchisq(dbl *retval, dbl chi2, dbl datapoints)
{
	*retval = dbl_nil;
	if (chi2 == dbl_nil || chi2 < 0)
		throw(MAL, "rmath.pchisq", "Wrong value for chi2");
	if (datapoints == dbl_nil || datapoints < 0)
		throw(MAL, "rmath.pchisq", "Wrong value for datapoints");
	*retval = pchisq(chi2, datapoints, 1, 0);
	return MAL_SUCCEED;
}

static str
rmath_bat_pchisq_cst(bat *retval, bat chi2, dbl datapoints)
{
	BAT *b, *bn;
	BATiter bi;
	BUN p, q;
	dbl r;

	if (datapoints == dbl_nil)
		throw(MAL, "rmathbat_pchisq_cst", "Parameter datapoints should not be nil");
	if (datapoints < 0)
		throw(MAL, "rmath.pchisq", "Wrong value for datapoints");

	if ((b = BATdescriptor(chi2)) == NULL) {
		throw(MAL, "pchisq", RUNTIME_OBJECT_MISSING);
	}
	bi = bat_iterator(b);
	bn = COLnew(b->hseqbase, TYPE_dbl, BATcount(b), TRANSIENT);
	if (bn == NULL) {
		BBPunfix(b->batCacheid);
		throw(MAL, "rmath.pchisq", MAL_MALLOC_FAIL);
	}
	BATloop(b, p, q) {
		dbl d = *(dbl *) BUNtail(bi, p);
		if (d == dbl_nil || d < 0) {
			BBPunfix(b->batCacheid);
			BBPreclaim(bn);
			throw(MAL, "rmath.pchisq", "Wrong value for chi2");
		}
		r = pchisq(d, datapoints, 1, 0);
		if (BUNappend(bn, &r, FALSE) != GDK_SUCCEED) {
			BBPunfix(b->batCacheid);
			BBPreclaim(bn);
			throw(MAL, "rmath.pchisq", GDK_EXCEPTION);
		}
	}
	*retval = bn->batCacheid;
	BBPkeepref(bn->batCacheid);
	BBPunfix(b->batCacheid);
	return MAL_SUCCEED;
}

static str
rmath_cst_pchisq_bat(bat *retval, dbl chi2, bat datapoints)
{
	BAT *b, *bn;
	BATiter bi;
	BUN p, q;
	dbl r;

	if (chi2 == dbl_nil)
		throw(MAL, "rmathbat_pchisq_cst", "Parameter chi2 should not be nil");
	if (chi2 < 0)
		throw(MAL, "rmath.pchisq", "Wrong value for chi2");
	if ((b = BATdescriptor(datapoints)) == NULL) {
		throw(MAL, "pchisq", RUNTIME_OBJECT_MISSING);
	}
	bi = bat_iterator(b);
	bn = COLnew(b->hseqbase, TYPE_dbl, BATcount(b), TRANSIENT);
	if (bn == NULL) {
		BBPunfix(b->batCacheid);
		throw(MAL, "rmath.pchisq", MAL_MALLOC_FAIL);
	}
	BATloop(b, p, q) {
		dbl datapoints = *(dbl *) BUNtail(bi, p);

		if (datapoints == dbl_nil || datapoints < 0) {
			BBPunfix(b->batCacheid);
			BBPreclaim(bn);
			throw(MAL, "rmath.pchisq", "Wrong value for datapoints");
		}
		r = pchisq(chi2, datapoints, 1, 0);
		if (BUNappend(bn, &r, FALSE) != GDK_SUCCEED) {
			BBPunfix(b->batCacheid);
			BBPreclaim(bn);
			throw(MAL, "rmath.pchisq", GDK_EXCEPTION);
		}
	}
	BBPkeepref(*retval = bn->batCacheid);
	BBPunfix(b->batCacheid);
	return MAL_SUCCEED;
}

static str
rmath_bat_pchisq_bat(bat *retval, bat chi2, bat datapoints)
{
	BAT *b, *c, *bn;
	dbl r, *chi2p, *datapointsp;
	size_t cnt = 0, i;

	if ((b = BATdescriptor(chi2)) == NULL) {
		throw(MAL, "pchisq", RUNTIME_OBJECT_MISSING);
	}
	if ((c = BATdescriptor(datapoints)) == NULL) {
		BBPunfix(b->batCacheid);
		throw(MAL, "pchisq", RUNTIME_OBJECT_MISSING);
	}
	bn = COLnew(b->hseqbase, TYPE_dbl, cnt = BATcount(b), TRANSIENT);
	if (bn == NULL) {
		BBPunfix(b->batCacheid);
		BBPunfix(c->batCacheid);
		throw(MAL, "rmath.pchisq", MAL_MALLOC_FAIL);
	}
	chi2p = (dbl *) Tloc(b, 0);
	datapointsp = (dbl *) Tloc(c, 0);
	for (i = 0; i < cnt; i++) {
		if (chi2p[i] == dbl_nil || chi2p[i] < 0) {
			BBPunfix(b->batCacheid);
			BBPunfix(c->batCacheid);
			BBPreclaim(bn);
			throw(MAL, "rmath.pchisq", "Wrong value for chi2");
		}
		if (datapointsp[i] == dbl_nil || datapointsp[i] < 0) {
			BBPunfix(b->batCacheid);
			BBPunfix(c->batCacheid);
			BBPreclaim(bn);
			throw(MAL, "rmath.pchisq", "Wrong value for datapoints");
		}
		r = pchisq(chi2p[i], datapointsp[i], 1, 0);
		if (BUNappend(bn, &r, FALSE) != GDK_SUCCEED) {
			BBPunfix(b->batCacheid);
			BBPunfix(c->batCacheid);
			BBPreclaim(bn);
			throw(MAL, "rmath.pchisq", GDK_EXCEPTION);
		}
	}
	BBPkeepref(*retval = bn->batCacheid);
	BBPunfix(b->batCacheid);
	BBPunfix(c->batCacheid);
	return MAL_SUCCEED;
}

str
rmathpchisq(dbl *retval, dbl *chi2, dbl *datapoints)
{
	return rmath_pchisq(retval, *chi2, *datapoints);
}

str
rmathbat_pchisq_cst(bat *retval, bat *chi2, dbl *datapoints)
{
	return rmath_bat_pchisq_cst(retval, *chi2, *datapoints);
}

str
rmathcst_pchisq_bat(bat *retval, dbl *chi2, bat *datapoints)
{
	return rmath_cst_pchisq_bat(retval, *chi2, *datapoints);
}

str
rmathbat_pchisq_bat(bat *retval, bat *chi2, bat *datapoints)
{
	return rmath_bat_pchisq_bat(retval, *chi2, *datapoints);
}
