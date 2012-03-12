Summary:   Samson Map Reduce Platform
Name:      samson
Version:    SAMSON_VERSION
Release:    SAMSON_RELEASE
License:   commercial
Group:     Applications/Engineering
Vendor:     Telefónica I+D
Packager:   Samson Development <samson-dev@tid.es>
URL:        http://wikis.hi.inet/samson
Source:    http://www.tid.es/samson-SAMSON_VERSION.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot
Requires: protobuf, ntp,tid-mongodb, kdchart
Requires(pre): shadow-utils

%description
SAMSON is a high-performance MapReduce platform that is designed for scaling tasks on a cluster of commodity hardware. This platform removes the need to develop a system for parallelizing and synchronizing work between servers, allowing a person to develop the analytical models needed to solve both current and future problems.  SAMSON extends the traditional MapReduce paradigm by allowing an external data source to stream data into and out of the cluster. This allows the system to handle any volume of data thus providing the ability to continuously process new inputs and output states without restriction.

%prep
%setup

%pre
getent group samson >/dev/null || groupadd -r samson
getent passwd samson >/dev/null || useradd -r -g samson -d /opt/samson -s /bin/bash -c 'SAMSON account' samson
exit 0

%post
bash /opt/samson/bin/samsonInitSetup
/sbin/chkconfig --add samson
/sbin/chkconfig --level 35 samson on
/etc/init.d/samson start

%preun
/etc/init.d/samson stop

%clean
rm -rf $RPM_BUILD_ROOT

%build
make release_all SAMSON_HOME=$RPM_BUILD_ROOT/opt/samson

%install
make SAMSON_HOME=$RPM_BUILD_ROOT/opt/samson install
mkdir -p $RPM_BUILD_ROOT/var/samson
mkdir -p $RPM_BUILD_ROOT/var/samson
mkdir -p $RPM_BUILD_ROOT/var/samson/etc
mkdir -p $RPM_BUILD_ROOT/etc/init.d
mkdir -p $RPM_BUILD_ROOT/etc/profile.d
cp etc/setup.txt $RPM_BUILD_ROOT/var/samson/etc
cp etc/init.d/samson.redhat $RPM_BUILD_ROOT/etc/init.d/samson
cp etc/profile.d/samson.sh  $RPM_BUILD_ROOT/etc/profile.d/samson.sh

echo "%%defattr(-, samson, samson, - )" > MANIFEST
(cd %{buildroot}; find . -type f -or -type l | sed -e s/^.// -e /^$/d) >>MANIFEST

%files -f MANIFEST

%changelog
