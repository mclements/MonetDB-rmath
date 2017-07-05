Name:		MonetDB-gsl
Version:	1.1
Release:	1%{?dist}
Summary:	MonetDB5 SQL interface to the gsl library

License:	MPLv2.0
URL:		https://www.monetdb.org/
Source0:	http://dev.monetdb.org/downloads/sources/%{name}-%{version}.tar.bz2

BuildRequires:	gsl-devel
BuildRequires:	MonetDB-devel >= 11.25.0
BuildRequires:	MonetDB5-server-devel >= 11.25.0
Requires:	MonetDB5-server%{?_isa} >= 11.26.0
Obsoletes:	MonetDB-gsl-MonetDB5

%description
MonetDB is a database management system that is developed from a
main-memory perspective with use of a fully decomposed storage model,
automatic index management, extensibility of data types and search
accelerators.  It also has an SQL frontend.

This package contains the interface to the GNU Scientific Library for
numerical analysis (gsl).

%prep
%autosetup


%build
%make_build


%install
rm -rf $RPM_BUILD_ROOT
%make_install


%files
%license COPYING
%{_libdir}/monetdb5/autoload/*_gsl.mal
%{_libdir}/monetdb5/createdb/*_gsl.sql
%{_libdir}/monetdb5/gsl.mal
%{_libdir}/monetdb5/lib_gsl.so

%changelog
* Wed Jul  5 2017 Sjoerd Mullender <sjoerd@acm.org> - 1.1-1
- Error checking.

* Tue Feb 14 2017 Sjoerd Mullender <sjoerd@acm.org>
- Separated gsl extension out from MonetDB.
