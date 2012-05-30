%define name samson
%define version SAMSON_VERSION
%define release SAMSON_RELEASE
%define owner samson

Summary:   Samson Map Reduce Platform
Name:      samson
Version:   %{version}
Release:   %{release}
License:   commercial
Group:     Applications/Engineering
Vendor:     Telefónica I+D
Packager:   Samson Development <samson-dev@tid.es>
URL:        http://wikis.hi.inet/samson
Source:    http://www.tid.es/samson-SAMSON_VERSION.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot
Requires: protobuf, ntp,tid-mongodb, samson-logserver == %{version}-%{release}, json-c
Requires(pre): shadow-utils

%description
SAMSON is a high-performance MapReduce platform that is designed for scaling
tasks on a cluster of commodity hardware. This platform removes the need to
develop a system for parallelizing and synchronizing work between servers,
allowing a person to develop the analytical models needed to solve both current
and future problems.  SAMSON extends the traditional MapReduce paradigm by
allowing an external data source to stream data into and out of the cluster.
This allows the system to handle any volume of data thus providing the ability
to continuously process new inputs and output states without restriction.

%prep
if [ -d $RPM_BUILD_ROOT/opt ]; then
   rm -rf $RPM_BUILD_ROOT
fi
%setup

%pre
getent group %{owner} >/dev/null || groupadd -r %{owner}
getent passwd %{owner} >/dev/null || useradd -r -g %{owner} -m -d /opt/samson -s /bin/bash -c 'SAMSON account' %{owner}
exit 0

%post
bash /opt/samson/bin/samsonInitSetup
/sbin/chkconfig --add samson
/sbin/chkconfig --level 35 samson on
chown -R %{owner}:%{owner} /opt/samson
chown -R %{owner}:%{owner} /var/samson
mkdir -p /var/log/samson
chown -R %{owner}:%{owner} /var/log/samson
/etc/init.d/samson start

%preun
/etc/init.d/samson stop

%clean
rm -rf $RPM_BUILD_ROOT

%build
make release_all SAMSON_HOME=$RPM_BUILD_ROOT/opt/samson

%install
make SAMSON_HOME=$RPM_BUILD_ROOT/opt/samson install_release_all
mkdir -p $RPM_BUILD_ROOT/var/samson
mkdir -p $RPM_BUILD_ROOT/var/samson/etc
mkdir -p $RPM_BUILD_ROOT/etc/init.d
mkdir -p $RPM_BUILD_ROOT/etc/profile.d
cp etc/setup.txt $RPM_BUILD_ROOT/var/samson/etc
cp etc/init.d/samson.redhat $RPM_BUILD_ROOT/etc/init.d/samson
cp etc/init.d/logserver.redhat $RPM_BUILD_ROOT/etc/init.d/logserver
chmod 755 $RPM_BUILD_ROOT/etc/init.d/samson
cp etc/profile.d/samson.sh  $RPM_BUILD_ROOT/etc/profile.d/samson.sh
chmod 755 $RPM_BUILD_ROOT/etc/profile.d/samson.sh


echo "%%defattr(-, samson, samson, - )" > MANIFEST
(cd %{buildroot}; find . -type f -or -type l | sed -e s/^.// -e /^$/d) >>MANIFEST
grep "samsonTopicMonitor\|delilah_graph\|samsonLocal" MANIFEST >> MANIFEST.samson-gui
grep "include\/\|lib\/\|samsonModule" MANIFEST >> MANIFEST.samson-devel
grep "logCat\|logServer\|logClient\|logserver" MANIFEST >> MANIFEST.samson-logserver
grep -v "samsonTopicMonitor\|delilah_graph\|samsonLocal\|include\|lib\/\|samsonModule\|logCat\|logServer\|logClient\|logserver" MANIFEST >> MANIFEST.samson

%files -f MANIFEST.samson

%changelog

%package gui
Summary: GUI tools for the SAMSON Platform
Group: Applications/Engineering
Requires: samson = %{version}-%{release}, kdchart
%description gui

%files gui -f MANIFEST.samson-gui

%package logserver
Summary: A simple log server
Group: Applications/Engineering
%description logserver
LogServer for the SAMSON Platform

%post logserver
mkdir -p /var/log/logserver
chown -R %{owner}:%{owner} /var/log/logserver
/sbin/chkconfig --add logserver
/sbin/chkconfig --level 35 logserver on
/etc/init.d/logserver start

%files logserver -f MANIFEST.samson-logserver

%package devel
Summary: Development files needed for the SAMSON platform
Group: Applications/Engineering
Requires: samson = %{version}-%{release}, protobuf-devel, gcc, gcc-c++, make, ncurses-devel, boost-devel, tid-mongodb-devel
%description devel

%files devel -f MANIFEST.samson-devel
