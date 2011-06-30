%description
The Samson Map Reduce Platform ...

%prep
%setup

%install

%clean


%files
/usr/local/bin/samson*
/usr/local/bin/delilah
/usr/local/include/samson/module/*.h
/opt/samson/etc/setup.txt

%changelog
* Tue Jun 28 2011 Ken Zangelin <kzangeli@tid.es> 
- New network library

* Tue Jun 28 2011 Andreu Urruela <andreu@tid.es> 
- Added streaming functionality

* Tue Jun 28 2011 J.Gregorio Escalada <jges@tid.es> 
- Improvements to Goyol

* Tue Jun 28 2011 Jorge Garay <jngaray@testabil.com>
- Fixed automatic test scripts
