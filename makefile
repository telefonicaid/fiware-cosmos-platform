
# ------------------------------------------------------------
# Environment variables
# ------------------------------------------------------------

ifndef SAMSON_HOME
SAMSON_HOME=/opt/samson
endif
ifndef SAMSON_WORKING
SAMSON_WORKING=/var/samson
endif
ifndef SAMSON_VERSION
SAMSON_VERSION=0.6.1
endif
ifndef SAMSON_RELEASE
SAMSON_RELEASE=$(shell svnversion . | sed -e 's/:.*//' -e 's/M//' )
endif
# Who to install samson as
ifndef SAMSON_OWNER
SAMSON_OWNER=samson
endif

DISTRO=$(shell lsb_release -is)
DISTRO_CODENAME=$(shell lsb_release -cs)

ifndef CPU_COUNT
	OS=$(shell uname -s)
	ifeq ($(OS),Darwin)
		#CPU_COUNT=$(shell sysctl hw.ncpu)
		CPU_COUNT=1
	else
		CPU_COUNT=$(shell cat /proc/cpuinfo | grep processor | wc -l)
	endif
endif

default: release

# ------------------------------------------------
# Prepare CMAKE
# ------------------------------------------------

prepare_release:
	mkdir BUILD_RELEASE || true
	cd BUILD_RELEASE; cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME)

prepare_strict:
	mkdir BUILD_STRICT || true
	cd BUILD_STRICT; cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DSTRICT=True -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME)

prepare_debug:
	mkdir BUILD_DEBUG || true
	cd BUILD_DEBUG; cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME)

prepare_coverage:
	mkdir BUILD_COVERAGE || true
	cd BUILD_COVERAGE; cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DCOVERAGE=True -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME)


prepare: prepare_release prepare_debug

# ------------------------------------------------
# Global install scripts
# ------------------------------------------------

i: install

install_debug: prepare_debug 
	make -C BUILD_DEBUG install
	mkdir -p $(SAMSON_HOME)/share/modules/moduletemplate
	cp README $(SAMSON_HOME)/share/README.txt
	cp modules/moduletemplate/CMakeLists.txt $(SAMSON_HOME)/share/modules/moduletemplate
	cp modules/moduletemplate/makefile $(SAMSON_HOME)/share/modules/moduletemplate
	cp modules/moduletemplate/module $(SAMSON_HOME)/share/modules/moduletemplate
	cp scripts/samsonModuleBootstrap $(SAMSON_HOME)/bin
	cp scripts/samsonProcessesSupervise $(SAMSON_HOME)/bin
	echo
	echo
	echo "Before starting Samson you need to run the following commands as root"
	echo "mkdir -p /var/samson"
	echo "chown -R $(SAMSON_OWNER):$(SAMSON_OWNER) $(SAMSON_WORKING)"

install: prepare_release
	make -C BUILD_RELEASE install
	make install_man
	mkdir -p $(SAMSON_HOME)/share/modules/moduletemplate
	cp README $(SAMSON_HOME)/share/README.txt
	cp modules/moduletemplate/CMakeLists.txt $(SAMSON_HOME)/share/modules/moduletemplate
	cp modules/moduletemplate/makefile $(SAMSON_HOME)/share/modules/moduletemplate
	cp modules/moduletemplate/module $(SAMSON_HOME)/share/modules/moduletemplate
	cp scripts/samsonModuleBootstrap $(SAMSON_HOME)/bin
	cp scripts/samsonProcessesSupervise $(SAMSON_HOME)/bin
	echo
	echo
	echo "Before starting Samson you need to run the following commands as root"
	echo "mkdir -p /var/samson"
	echo "chown -R $(SAMSON_OWNER):$(SAMSON_OWNER) $(SAMSON_WORKING)"

install_man: man
	cp -r BUILD_RELEASE/man $(SAMSON_HOME)/

# Install the supporting script/files that go in the OS directories
install_scripts:
	cp etc/profile.d 
 
clean:
	make -C BUILD_DEBUG clean
	make -C BUILD_RELEASE clean	

# ------------------------------------------------
# RELEASE Version
# ------------------------------------------------


release: prepare_release
	make -C BUILD_RELEASE -j $(CPU_COUNT)

# ------------------------------------------------
# RELEASE Version
# ------------------------------------------------


strict: strict_compilation
strict_compilation: prepare_strict
	make -C BUILD_STRICT -j $(CPU_COUNT)

# ------------------------------------------------
# DEBUG Version
# ------------------------------------------------

debug: prepare_debug
	make -C BUILD_DEBUG -j $(CPU_COUNT)

# ------------------------------------------------
# DEBUG Version
# ------------------------------------------------

coverage: prepare_coverage
	make -C BUILD_COVERAGE -j $(CPU_COUNT)
	lcov -d BUILD_COVERAGE -z	
	BUILD_COVERAGE/apps/unitTest/unitTest
	mkdir -p coverage
	lcov -d BUILD_COVERAGE --capture --output-file coverage/samson.info
	lcov -r coverage/samson.info "/usr/include/*" -o coverage/samson.info
	lcov -r coverage/samson.info "/usr/local/include/*" -o coverage/samson.info
	genhtml -o coverage coverage/samson.info

# ------------------------------------------------
# Testing
# ------------------------------------------------

test: ctest itest

ctest: debug
	make test -C BUILD_DEBUG

test_local_processes:
	./scripts/test_local_processes

# package: release
#	make -C BUILD_RELEASE package 

# ------------------------------------------------
# Debug Install / Distribute
# ------------------------------------------------

pdi: debug
	make -C BUILD_DEBUG install

pdid: pdi
	./scripts/samsonDist

di:	debuginstall

debuginstall: debug
	make -C BUILD_DEBUG install

distribute: install
ifndef SAMSON_CLUSTER
	echo "The environment variable SAMSON_CLUSTER is not defined, unable to distribute"
	exit 1
endif
	./scripts/samsonDist

d:	distribute

dd:	debugdistribute

debugdistribute: debuginstall
ifndef SAMSON_CLUSTER
	echo "The environment variable SAMSON_CLUSTER is not defined, unable to distribute"
	exit 1
endif
	./scripts/samsonDist

testdistribute: install
	./scripts/samsonTestDistributer


# ------------------------------------------------
# Other
# ------------------------------------------------

xcode:	
	./scripts/prepareXcode

eclipse:	
	./scripts/prepareEclipse

reset:
	rm -Rf BUILD_DEBUG
	rm -Rf BUILD_RELEASE
	rm -Rf BUILD_DEBUG_COVERAGE
	rm -Rf xcode_proj
	rm -f libs/common/samson.pb.*
	rm -f libs/data/data.pb.*
	rm -Rf /usr/local/include/samson
	rm -f testing/module_test/Module.*
	rm -f /etc/init/samson.conf
	rm -f /etc/profile.d/samson.sh

cleansvn: 
	for file in `svn status | grep ? | awk '{print $$2}'`; do rm -rf $$file; done

# Remove /opt/samson to guarantee a clean build
cleanhudson:
	rm -rf /opt/samson

#modules: release
#	make release -C modules

#modules_debug: debug
#	make -C modules


qt:
	make -C build/apps/delilahQt

clean_qt:
	make -C build/apps/delilahQt clean


v: di
	valgrind -v  --leak-check=full --track-origins=yes --show-reachable=yes  samsonLocal  2> output_valgrind_samsonLocal


clear_ipcs:
	for i in `sudo ipcs -m | grep -v key  | grep -v Memory  |  awk '{print $$2}'`; do sudo ipcrm -m $$i; done

set_ssm_linux:
	sudo sysctl -w kernel.shmmax=64000000

rpm:
	mkdir -p ~/rpmbuild/{BUILD,RPMS,S{OURCE,PEC,RPM}S}
	rm -f ~/rpmbuild/SOURCES/samson-$(SAMSON_VERSION).tar.gz
	tar cz --transform 's,^./,samson-$(SAMSON_VERSION)/,' .  --show-transformed-names --exclude=*.svn* -f ~/rpmbuild/SOURCES/samson-$(SAMSON_VERSION).tar.gz
	sed -e "s/SAMSON_VERSION/$(SAMSON_VERSION)/g" -e "s/SAMSON_RELEASE/$(SAMSON_RELEASE)/g" rpm/samson.spec > rpm/samson-$(SAMSON_VERSION).spec
	rpmbuild -bb rpm/samson-$(SAMSON_VERSION).spec
    
publish_rpm: rpm
	rpm/rpm-sign.exp ~/rpmbuild/RPMS/x86_64/samson-$(SAMSON_VERSION)-$(SAMSON_RELEASE).x86_64.rpm
	rsync  -v  ~/rpmbuild/RPMS/x86_64/samson-$(SAMSON_VERSION)-$(SAMSON_RELEASE).x86_64.rpm repo@samson09.hi.inet:/var/repository/redhat/6/x86_64
	ssh repo@samson09.hi.inet createrepo -q -d /var/repository/redhat/6/x86_64

# currently the deb scripts require Samson be installed before 
# the package can be generated. Using SAMSON_HOME we can override
# the default install location so as to not trash a live installation
deb:
	rm -rf package/deb
	sed -e  "s/SAMSON_VERSION/$(SAMSON_VERSION)/"  -e "s/SAMSON_RELEASE/$(SAMSON_RELEASE)/" -e "s/DISTRO_CODENAME/$(DISTRO_CODENAME)/" CHANGELOG > debian/changelog
	# Right now *everything* comes in a single package
	dpkg-buildpackage -b
	mkdir -p package/deb
	mv ../samson_$(SAMSON_VERSION).$(SAMSON_RELEASE)* package/deb

publish_deb: deb
	# Upload the files. A cron job on the server will include them in the repository
	scp package/deb/* repo@samson09:/var/repository/ubuntu/incoming/$(DISTRO_CODENAME)

man:
	if [ ! -d BUILD_RELEASE ]; then \
		echo "execute make release before trying to build the manual pages"; \
	fi
	mkdir -p BUILD_RELEASE/man/man1
	mkdir -p BUILD_RELEASE/man/man7
	help2man --name="Samson worker"         --no-info --section=1 --manual=Samson ./BUILD_RELEASE/apps/samsonWorker/samsonWorker     > ./BUILD_RELEASE/man/man1/samsonWorker.1
	help2man --name="Samson setup"          --no-info --section=1 --manual=Samson ./BUILD_RELEASE/apps/samsonSetup/samsonSetup       > ./BUILD_RELEASE/man/man1/samsonSetup.1
	help2man --name="Samson platform interaction shell" --no-info --section=1 --manual=Samson ./BUILD_RELEASE/apps/delilah/delilah                > ./BUILD_RELEASE/man/man1/delilah.1
	cp man/samson-*.7 ./BUILD_RELEASE/man/man7

uninstall:
	if [ -f BUILD_RELEASE/install_manifest.txt ]; then \
	  xargs rm -f  < BUILD_RELEASE/install_manifest.txt; \
	fi

uninstall_debug:
	if [ -f BUILD_DEBUG/install_manifest.txt ]; then \
	  xargs rm -f  < BUILD_DEBUG/install_manifest.txt; \
	fi
	

packages: install man rpm deb

.PHONY : modules
.PHONY : man
.PHONY : rpm
.PHONY : deb

init_home:
	mkdir -p $(SAMSON_HOME)
	mkdir -p $(SAMSON_WORKING)
	mkdir -p /var/log/samson
	chown -R $(SAMSON_OWNER):$(SAMSON_OWNER) $(SAMSON_HOME) $(SAMSON_WORKING) /var/log/samson

#vim: noexpandtab
