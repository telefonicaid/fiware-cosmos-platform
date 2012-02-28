Summary: Monitoring Tool
Name: monitoring 
Version: 0.1
License:  def, jjgazol 
# TODO: Find the standard group that fits the Visualization Tools
Group: Applications/Internet
Source0: monitoring.zip
# Warning! Despite the name, the build-root dir will be the INSTALL-root dir (coincident in this case, but not necessarily)
BuildRoot: %{_topdir}/BUILD
# RPM requirements
Requires: httpd



%description
This RPM contains the PSSuite visualization tool

%define debug_package %{nil}

# Do not check unpackaged files
%undefine __check_files.


# -------------------------------------------------------------------------------------------- #
# prep section, setup macro:
# -------------------------------------------------------------------------------------------- #
# removes previous build files, unpack source zip file and cd into the source dir
#  -q: keep quiet (do not output unpacking info)
#  -n <name>: specified dir name to replace default one (<name>-<version>)
%prep
%setup -q -n monitoring


%build
#No build necessary, all code is set into app path

# -------------------------------------------------------------------------------------------- #
# install section:
# -------------------------------------------------------------------------------------------- #
# Install the Monitoring project and Apache config file
%install
# Check dir existence
if [ ! -d $RPM_BUILD_ROOT%{_psappdir} ]
then
    mkdir -p $RPM_BUILD_ROOT%{_psappdir}
fi

# Copy the Viz Tool to its destination
cp -R %{_topdir}/BUILD/ps-tools $RPM_BUILD_ROOT%{_psappdir}/.



# Check Apache conf dir existence
if [ ! -d $RPM_BUILD_ROOT%{_apacheconfdir} ]
then
    mkdir -p $RPM_BUILD_ROOT%{_apacheconfdir}
fi


# -------------------------------------------------------------------------------------------- #
# post-install section:
# -------------------------------------------------------------------------------------------- #
%post

# Restart Apache
/etc/init.d/httpd restart

echo 'Monitor Tool has been installed!'

# -------------------------------------------------------------------------------------------- #
# post-uninstall section:
# -------------------------------------------------------------------------------------------- #
%postun
# Restart Apache
/etc/init.d/httpd restart

echo 'Monitor Tool has been uninstalled!'

%clean
rm -rf $RPM_BUILD_ROOT/*

%files
%defattr(-, perserver, perserver)
# Specify application dir
%{_psappdir}/ps-bigData
# Specify config file




