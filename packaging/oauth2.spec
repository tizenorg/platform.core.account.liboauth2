Name:       oauth2
Summary:    Tizen oauth 2.0 Framework
Version:    0.0.1
Release:    1
Group:      Social & Content/API
License:    Apache-2.0
Source0:    oauth2-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(bundle)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(libcurl)
BuildRequires:  pkgconfig(efl-extension)
#BuildRequires:  pkgconfig(ewebkit2)
BuildRequires:  pkgconfig(chromium-efl)
BuildRequires:  pkgconfig(json-glib-1.0)
BuildRequires:  pkgconfig(elementary)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Tizen oauth 2.0 Framework

%package devel
Summary:    Development files for %{name}
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}
%description devel
Development files for %{name}

%prep
%setup -q

%build
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

export CFLAGS="${CFLAGS} -fPIC -fvisibility=hidden"

%if "%{?tizen_profile_name}" == "wearable"
cmake . \
-DCMAKE_INSTALL_PREFIX=/usr \
-DINCLUDEDIR=%{_includedir} \
-DLIBDIR=%{_libdir} \
-DEXCLUDE_EWK_ERR="YES"

%else
cmake . \
-DCMAKE_INSTALL_PREFIX=/usr \
-DINCLUDEDIR=%{_includedir} \
-DLIBDIR=%{_libdir} \

%endif

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

rm -rf %{buildroot}/usr/lib/oauth2

%postun -p /sbin/ldconfig


%files
%manifest oauth2.manifest
%defattr(-,root,root,-)
%{_libdir}/*.so.*


%files devel
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_includedir}/*.h
%{_libdir}/pkgconfig/oauth2.pc
