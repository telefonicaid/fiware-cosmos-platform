%description
The Samson Map Reduce Platform ...

%prep
%setup -q

%build

%install
rm -rf /opt/samson
mkdir -p /opt/samson/bin
mkdir -p /opt/samson/lib
mkdir -p /opt/samson/include/samson/module
mkdir -p /opt/samson/man/man1
mkdir -p /opt/samson/man/man3

install -s -m 755 bin/samsonCat                  /opt/samson/bin/
install -s -m 755 bin/samsonController           /opt/samson/bin/
install -s -m 755 bin/samsonData                 /opt/samson/bin/
install -s -m 755 bin/samsonKiller               /opt/samson/bin/
install -s -m 755 bin/samsonLocal                /opt/samson/bin/
install -s -m 755 bin/samsonModuleParser         /opt/samson/bin/
install -s -m 755 bin/samsonSetup                /opt/samson/bin/
install -s -m 755 bin/samsonSpawner              /opt/samson/bin/
install -s -m 755 bin/samsonWorker               /opt/samson/bin/
install -s -m 755 bin/delilah*                   /opt/samson/bin/
install    -m 444 lib/libsamson*                 /opt/samson/lib/
install    -m 444 include/samson/module/*.h      /opt/samson/include/samson/module/
#install -m 644 samson.1                         /opt/samson/man/man1/samson.1

%clean
rm -rf /opt/samson

%files
%defattr(-,root,root)
#%doc README TODO COPYING ChangeLog

%changelog
* Tue Jun 28 2011 Ken Zangelin <kzangeli@tid.es> 
- New network library

* Tue Jun 28 2011 Andreu Urruela <andreu@tid.es> 
- Added streaming functionality

* Tue Jun 28 2011 J.Gregorio Escalada <jges@tid.es> 
- Improvements to Goyol

* Tue Jun 28 2011 Jorge Garay <jngaray@testabil.com>
- Fixed automatic test scripts
