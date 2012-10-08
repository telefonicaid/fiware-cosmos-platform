
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
SAMSON_VERSION=0.7.0
endif
ifndef SAMSON_RELEASE
	# Use the current timestamp guarantee unique builds
	SAMSON_RELEASE=$(shell echo `date +%Y%M%d%H%M`_git)
endif
# Who to install samson as
ifndef SAMSON_OWNER
SAMSON_OWNER=samson
endif

# Repository Server
ifndef REPO_SERVER
REPO_SERVER=samson09.hi.inet
endif

DISTRO:=$(shell lsb_release -is)
DISTRO_CODENAME:=$(shell lsb_release -cs)

ifndef CPU_COUNT
	OS=$(uname -s)
	ifeq ($(OS),Darwin)
		CPU_COUNT=$(shell sysctl hw.ncpu | cut -f 2 -d" ")
	endif
	ifeq ($(OS),Linux)
		CPU_COUNT:=$(shell cat /proc/cpuinfo | grep processor | wc -l)
	else
		CPU_COUNT=8
	endif
endif

# Port number to handle tests for multiple version simultaneously
ifndef SAMSON_WORKER_PORT_ENV
SAMSON_WORKER_PORT_ENV=1324
endif

# Web port number to handle tests for multiple version simultaneously
ifndef SAMSON_WORKER_WEB_PORT_ENV
SAMSON_WORKER_WEB_PORT_ENV=1202
endif

# Log port number to handle tests for multiple version simultaneously
ifndef SAMSON_LOG_PORT_ENV
SAMSON_LOG_PORT_ENV=6001
endif

# Log query port number to handle tests for multiple version simultaneously
ifndef SAMSON_LOGQUERY_PORT_ENV
SAMSON_LOGQUERY_PORT_ENV=6000
endif

# Log dir to handle tests for multiple version simultaneously
ifndef LOGSERVER_LOG_DIR
LOGSERVER_LOG_DIR=/var/log/logserver
endif

# Zookeeper server hostname
ifndef ZOOKEEPER_SERVER
ZOOKEEPER_SERVER=localhost
endif

# Log dir to handle tests for multiple version simultaneously
ifndef ZOOKEEPER_PORT
ZOOKEEPER_PORT=2181
endif

# Lint for C/C++ code
ifndef CPPLINT
	CPPLINT:=$(shell pwd)/scripts/cpplint.py
endif

# ------------------------------------------------
# Default compilation mode
# ------------------------------------------------

default: release

# ------------------------------------------------
# PREPARING
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
	cd BUILD_COVERAGE; cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_MODULES=True -DCOVERAGE=True -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME)

prepare_release_all:
	mkdir BUILD_RELEASE_ALL || true
	cd BUILD_RELEASE_ALL; cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_MODULES=True -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME)

prepare_debug_all:
	mkdir BUILD_DEBUG_ALL || true
	cd BUILD_DEBUG_ALL; cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_MODULES=True -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME)

prepare: prepare_release prepare_debug prepare_coverage prepare_release_all prepare_debug_all

# ------------------------------------------------
# BUILDING
# ------------------------------------------------

release: prepare_release
	make -C BUILD_RELEASE -j $(CPU_COUNT)

debug: prepare_debug
	make -C BUILD_DEBUG -j $(CPU_COUNT)

all: release_all
release_all: prepare_release_all
	make -C BUILD_RELEASE_ALL -j $(CPU_COUNT)

da: debug_all
debug_all: prepare_debug_all
	make -C BUILD_DEBUG_ALL -j $(CPU_COUNT)

coverage: prepare_coverage
	make -C BUILD_COVERAGE -j $(CPU_COUNT)

strict: strict_compilation
strict_compilation: prepare_strict
	make -C BUILD_STRICT -j $(CPU_COUNT)


# ------------------------------------------------
# INSTALLING
# ------------------------------------------------

i: install
install: install_release
install_release: release
	make -C BUILD_RELEASE install
	make install_man_release
	scripts/postInstall $(SAMSON_HOME) $(SAMSON_WORKING) $(SAMSON_OWNER)

di:	install_debug
install_debug: debug
	make -C BUILD_DEBUG install -j $(CPU_COUNT)
	mkdir -p $(SAMSON_HOME)/share/modules/moduletemplate
	scripts/postInstall $(SAMSON_HOME) $(SAMSON_WORKING) $(SAMSON_OWNER)

ria: install_release_all
install_release_all: release_all
	make -C BUILD_RELEASE_ALL install
	make install_man_release_all
	scripts/postInstall $(SAMSON_HOME) $(SAMSON_WORKING) $(SAMSON_OWNER)

dai: install_debug_all
dia: install_debug_all
install_debug_all: debug_all
	make -C BUILD_DEBUG_ALL install
	scripts/postInstall $(SAMSON_HOME) $(SAMSON_WORKING) $(SAMSON_OWNER)

install_coverage: coverage
	make -C BUILD_COVERAGE install -j $(CPU_COUNT)
	mkdir -p  $(SAMSON_WORKING)/etc
	cp etc/setup.txt.test $(SAMSON_WORKING)/etc/setup.txt

install_man_release: man_release
	cp -r BUILD_RELEASE/man $(SAMSON_HOME)/

install_man_release_all: man_release_all
	cp -r BUILD_RELEASE_ALL/man $(SAMSON_HOME)/

clean:
	if [ -d BUILD_RELEASE ]; then \
		make -C BUILD_RELEASE clean; \
	fi
	if [ -d BUILD_DEBUG ]; then \
		make -C BUILD_DEBUG clean; \
	fi
	if [ -d BUILD_RELEASE_ALL ]; then \
		make -C BUILD_RELEASE_ALL clean; \
	fi
	if [ -d BUILD_DEBUG_ALL ]; then \
		make -C BUILD_DEBUG_ALL clean; \
	fi
	if [ -d BUILD_COVERAGE ]; then \
		make -C BUILD_COVERAGE clean; \
	fi
	rm -rf BUILD_DEBUG
	rm -rf BUILD_RELEASE
	rm -rf BUILD_COVERAGE


# ------------------------------------------------
# Platform Code Coverage Version
# ------------------------------------------------

clean_samson:
	if [ "${SAMSON_HOME}" != "/opt/samson" ]; then \
		rm -rf ${SAMSON_HOME}/*; \
	fi
	if [ "${SAMSON_WORKING}" != "/var/samson" ]; then \
		rm -rf ${SAMSON_WORKING}/log; \
		rm -rf ${SAMSON_WORKING}/blocks; \
	fi

test_init:
	kill `ps -efl | fgrep "samsonWorker -port ${SAMSON_WORKER_PORT_ENV}" | fgrep -v fgrep | awk '{print $$4}'` || true
	#killall samsonWorker || true
	kill `ps -efl | fgrep "logServer -port ${SAMSON_LOG_PORT_ENV}" | fgrep -v fgrep | awk '{print $$4}'` || true
	bash -x scripts/lib_shared_memory.scr || true
	lcov --directory BUILD_COVERAGE --zerocounters	
	logServer -port ${SAMSON_LOG_PORT_ENV} -dir ${LOGSERVER_LOG_DIR} -query_port ${SAMSON_LOGQUERY_PORT_ENV}
	samsonWorker -port ${SAMSON_WORKER_PORT_ENV} -web_port ${SAMSON_WORKER_WEB_PORT_ENV} -log_port ${SAMSON_LOG_PORT_ENV} -log_classic ${SAMSON_LOG_LEVELS} -zk ${ZOOKEEPER_SERVER}:${ZOOKEEPER_PORT}

test_teardown:
	kill `ps -efl | fgrep "samsonWorker -port ${SAMSON_WORKER_PORT_ENV}" | fgrep -v fgrep | awk '{print $$4}'` || true
	kill `ps -efl | fgrep "logServer -port ${SAMSON_LOG_PORT_ENV}" | fgrep -v fgrep | awk '{print $$4}'` || true
	bash -x scripts/lib_shared_memory.scr

test_report:
	mkdir -p coverage
	lcov --directory BUILD_COVERAGE --capture --output-file coverage/samson.info
	lcov -r coverage/samson.info "/usr/include/*" -o coverage/samson.info
	lcov -r coverage/samson.info "/usr/local/include/*" -o coverage/samson.info
	lcov -r coverage/samson.info "BUILD_COVERAGE/modules/*" -o coverage/samson.info
	lcov -r coverage/samson.info "modules/*" -o coverage/samson.info
	lcov -r coverage/samson.info "/opt/local/include/google/*" -o coverage/samson.info
	genhtml -o coverage coverage/samson.info

run_coverage: install_coverage test_coverage test_report

# There is a problem with coverage tool for mac.. please do use this output a mac environment

run_mac_coverage: install_coverage
	export GCOV_PREFIX=$(PWD)
	make begin_mac_coverage
	make process_mac_coverage
	make finish_mac_coverage

begin_mac_coverage:	
	rm -Rf CMakeFiles
	rm -Rf coverage
	scripts/prepare_mac_coverage.sh				  
	lcov --directory CMakeFiles --zerocounters
	echo "NOTE: PLEASE MAKE SURE YOU RUN  export GCOV_PREFIX=$$PWD"

process_mac_coverage:
	kill `ps -efl | fgrep "samsonWorker -port ${SAMSON_WORKER_PORT_ENV}" | fgrep -v fgrep | awk '{print $$4}'` || true
	samsonWorker -port ${SAMSON_WORKER_PORT_ENV} -web_port ${SAMSON_WORKER_WEB_PORT_ENV} -log_port ${SAMSON_LOG_PORT_ENV} -log_classic ${SAMSON_LOG_LEVELS}
	make test_coverage
	kill `ps -efl | fgrep "samsonWorker -port ${SAMSON_WORKER_PORT_ENV}" | fgrep -v fgrep | awk '{print $$4}'` || true

finish_mac_coverage:
	mkdir -p coverage
	lcov --directory CMakeFiles --capture --output-file coverage/samson.info
	lcov -r coverage/samson.info "/usr/include/*" -o coverage/samson.info
	lcov -r coverage/samson.info "/usr/local/include/*" -o coverage/samson.info
	lcov -r coverage/samson.info "BUILD_COVERAGE/modules/*" -o coverage/samson.info
	lcov -r coverage/samson.info "modules/*" -o coverage/samson.info
	lcov -r coverage/samson.info "/opt/local/include/google/*" -o coverage/samson.info
	lcov -r coverage/samson.info "extern/*" -o coverage/samson.info
	genhtml -o coverage coverage/samson.info

# ------------------------------------------------
# Testing
# ------------------------------------------------

test: ctest
ctest: debug_all
	cp BUILD_DEBUG_ALL/modules/core/txt/libtxt.so /tmp
	cp apps/unitTest/delilah/words_input.txt /tmp
	# To configure DART_TIMEOUT to 400 instead of 1500. 
	sed 's/1500/400/' BUILD_DEBUG_ALL/DartConfiguration.tcl > /tmp/DartConfiguration.tcl; mv /tmp/DartConfiguration.tcl BUILD_DEBUG_ALL/DartConfiguration.tcl
	mkdir -p /tmp/dir_test
	cp apps/unitTest/delilah/words_input.txt /tmp/dir_test
	make test -C BUILD_DEBUG_ALL ARGS="-D ExperimentalTest" TEST_VERBOSE=1
	# Trying to clean up the shared memory
	bash -x scripts/lib_shared_memory.scr || true
	# Enable core dumps for any potential SEGVs
	ulimit -c unlimited && BUILD_DEBUG_ALL/apps/unitTest/unitTest ${SAMSON_LOG_LEVELS} --gtest_output=xml:BUILD_DEBUG_ALL/samson_test.xml
	# Convert "disabled" tests to "skipped" tests so we can keep track in Jenkins
	sed -i -e 's/disabled/skipped/' BUILD_DEBUG_ALL/samson_test.xml

unit_test: debug_all
	cp BUILD_DEBUG_ALL/modules/core/txt/libtxt.so /tmp
	cp apps/unitTest/delilah/words_input.txt /tmp
	# To configure DART_TIMEOUT to 400 instead of 1500. 
	sed 's/1500/400/' BUILD_DEBUG_ALL/DartConfiguration.tcl > /tmp/DartConfiguration.tcl; mv /tmp/DartConfiguration.tcl BUILD_DEBUG_ALL/DartConfiguration.tcl
	mkdir -p /tmp/dir_test
	cp apps/unitTest/delilah/words_input.txt /tmp/dir_test
	# Trying to clean up the shared memory
	bash -x scripts/lib_shared_memory.scr || true
	# Enable core dumps for any potential SEGVs
	ulimit -c unlimited && BUILD_DEBUG_ALL/apps/unitTest/unitTest  ${SAMSON_LOG_LEVELS} --gtest_shuffle --gtest_output=xml:BUILD_DEBUG_ALL/samson_test.xml
	# Convert "disabled" tests to "skipped" tests so we can keep track in Jenkins
	sed -i -e 's/disabled/skipped/' BUILD_DEBUG_ALL/samson_test.xml

test_coverage:
	cp BUILD_COVERAGE/modules/core/txt/libtxt.so /tmp
	cp apps/unitTest/delilah/words_input.txt /tmp
	mkdir -p /tmp/dir_test
	cp apps/unitTest/delilah/words_input.txt /tmp/dir_test
	# To configure DART_TIMEOUT to 400 instead of 1500. 
	sed 's/1500/400/' BUILD_COVERAGE/DartConfiguration.tcl > /tmp/DartConfiguration.tcl; mv /tmp/DartConfiguration.tcl BUILD_COVERAGE/DartConfiguration.tcl
	# Disable functional testing until we can sort out zookeeper
	#make test -C BUILD_COVERAGE ARGS="-D ExperimentalTest" TEST_VERBOSE=1 || true
	# Trying to clean up the shared memory
	bash -x scripts/lib_shared_memory.scr || true
	# Enable core dumps for any potential SEGVs
	ulimit -c unlimited && BUILD_COVERAGE/apps/unitTest/unitTest  ${SAMSON_LOG_LEVELS} --gtest_output=xml:BUILD_COVERAGE/samson_test.xml || true
	# Convert "disabled" tests to "skipped" tests so we can keep track in Jenkins
	sed -i -e 's/disabled/skipped/' -e 's,\(<testcase name="\)DISABLED_\(.*status="notrun".*\) />,\1\2>\n      <skipped/>\n    </testcase>,' BUILD_COVERAGE/samson_test.xml


test_samsonWorker: install_debug
	scripts/samsonTest test/samsonWorker
test_delilah: install_debug
	scripts/samsonTest test/delilah

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


distribute: install_debug_all
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
	mkdir xcode_proj || true
	cd xcode_proj; cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_MODULES=False -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME) -G Xcode

eclipse:	
	./scripts/prepareEclipse

reset:
	rm -Rf BUILD_DEBUG
	rm -Rf BUILD_DEBUG_ALL
	rm -Rf BUILD_STRICT
	rm -Rf BUILD_RELEASE
	rm -Rf BUILD_RELEASE_ALL
	rm -Rf BUILD_COVERAGE
	rm -f libs/common/samson.pb.*
	rm -f libs/data/data.pb.*
	rm -Rf /usr/local/include/samson || true
	rm -f testing/module_test/Module.*
	rm -f /etc/init/samson.conf || true
	rm -f /etc/profile.d/samson.sh || true

reset_xcode:
	rm -Rf xcode_proj

# Remove /opt/samson to guarantee a clean build
cleanhudson:
	rm -rf /opt/samson

#modules: release
#	make release -C modules

#modules_debug: debug
#	make -C modules


v: valgrind
valgrind: di
	valgrind -v  --leak-check=full --track-origins=yes --show-reachable=yes  samsonLocal  2> output_valgrind_samsonLocal


clear_ipcs:
	for mem in `sudo ipcs -m  | awk '{ if ($$6 == 0) print $$2;}'`; do ipcrm -m $$mem; done

clear_ipcs_mac:
	for mem in `sudo ipcs -m  | awk '{ if ($$1 == "m") print $$2;}'`; do ipcrm -m $$mem; done

set_ssm_linux:
	sudo sysctl -w kernel.shmmax=64000000

publish_packages:
	# Generic task to invoke the the correct package step depending on the OS
	echo "RELEASE $(SAMSON_RELEASE)"
	if [ "$(DISTRO)" = "Ubuntu" ]; then \
		make publish_deb; \
	else  \
		if [ "$(DISTRO)" = "CentOS" ] || [ "$(DISTRO)" = "RedHatEnterpriseServer" ] ; then \
			make publish_rpm; \
		else \
			echo "Unknown platform $(DISTRO)"; \
		fi; \
	fi

# Create a source tarball
# Note that this only uses the checked in source, any non-committed code 
# will not be archived
tarball:
	git archive --format=tar --prefix=./samson-$(SAMSON_VERSION)/ HEAD | gzip > samson-$(SAMSON_VERSION).tar.gz

rpm: clean
	mkdir -p ~/rpmbuild/{BUILD,RPMS,S{OURCE,PEC,RPM}S}
	rm -f ~/rpmbuild/SOURCES/samson-$(SAMSON_VERSION).tar.gz
	# Generate a tarball from the latest checked in code
	git archive --format=tar --prefix=./samson-$(SAMSON_VERSION)/ HEAD | gzip > ~/rpmbuild/SOURCES/samson-$(SAMSON_VERSION).tar.gz
	rpmbuild -bb rpm/samson.spec --define 'version $(SAMSON_VERSION)' --define 'release $(SAMSON_RELEASE)'

publish_rpm: rpm
	rpm/rpm-sign.exp ~/rpmbuild/RPMS/x86_64/samson-*$(SAMSON_VERSION)-$(SAMSON_RELEASE).x86_64.rpm
	rsync  -v  ~/rpmbuild/RPMS/x86_64/samson-*$(SAMSON_VERSION)-$(SAMSON_RELEASE).x86_64.rpm repo@$(REPO_SERVER):/var/repository/redhat/6/x86_64
	ssh repo@$(REPO_SERVER) createrepo -q -d /var/repository/redhat/6/x86_64

# currently the deb scripts require Samson be installed before 
# the package can be generated. Using SAMSON_HOME we can override
# the default install location so as to not trash a live installation
deb:
	rm -rf package/deb
	sed -e  "s/SAMSON_VERSION/$(SAMSON_VERSION)/"  -e "s/SAMSON_RELEASE/$(SAMSON_RELEASE)-$(DISTRO_CODENAME)/" -e "s/DISTRO_CODENAME/$(DISTRO_CODENAME)/" CHANGELOG > debian/changelog
	dpkg-buildpackage -b
	mkdir -p package/deb
	mv ../samson*$(SAMSON_VERSION).$(SAMSON_RELEASE)* package/deb

publish_deb: deb
	# Upload the files. A cron job on the server will include them in the repository
	scp package/deb/* repo@$(REPO_SERVER):/var/repository/ubuntu/incoming/$(DISTRO_CODENAME)

man_release:
	if [ ! -d BUILD_RELEASE ]; then \
		echo "execute make release before trying to build the manual pages"; \
	fi
	mkdir -p BUILD_RELEASE/man/man1
	mkdir -p BUILD_RELEASE/man/man7
	help2man --name="Samson worker"  --no-info --section=1 --manual=Samson ./BUILD_RELEASE/apps/samsonWorker/samsonWorker     > ./BUILD_RELEASE/man/man1/samsonWorker.1
	help2man --name="Samson setup"   --no-info --section=1 --manual=Samson ./BUILD_RELEASE/apps/samsonSetup/samsonSetup       > ./BUILD_RELEASE/man/man1/samsonSetup.1
	help2man --name="Delilah"  --no-info --section=1 --manual=Samson ./BUILD_RELEASE/apps/delilah/delilah > ./BUILD_RELEASE_ALL/man/man1/delilah.1
	cp man/samson-*.7 ./BUILD_RELEASE/man/man7

man_release_all: release_all
	if [ ! -d BUILD_RELEASE_ALL ]; then \
		echo "execute make release_all before trying to build the manual pages"; \
	fi
	mkdir -p BUILD_RELEASE_ALL/man/man1
	mkdir -p BUILD_RELEASE_ALL/man/man7
	help2man --name="Samson worker" --no-info --section=1 --manual=Samson ./BUILD_RELEASE_ALL/apps/samsonWorker/samsonWorker > ./BUILD_RELEASE_ALL/man/man1/samsonWorker.1
	help2man --name="Samson setup"  --no-info --section=1 --manual=Samson ./BUILD_RELEASE_ALL/apps/samsonSetup/samsonSetup > ./BUILD_RELEASE_ALL/man/man1/samsonSetup.1
	help2man --name="Delilah"  --no-info --section=1 --manual=Samson ./BUILD_RELEASE_ALL/apps/delilah/delilah > ./BUILD_RELEASE_ALL/man/man1/delilah.1
	cp man/samson-*.7 ./BUILD_RELEASE_ALL/man/man7

uninstall:
	if [ -f BUILD_RELEASE/install_manifest.txt ]; then \
	  xargs rm -f  < BUILD_RELEASE/install_manifest.txt; \
	fi

uninstall_debug:
	if [ -f BUILD_DEBUG/install_manifest.txt ]; then \
	  xargs rm -f  < BUILD_DEBUG/install_manifest.txt; \
	fi
	

.PHONY : modules
.PHONY : man
.PHONY : rpm
.PHONY : deb

init_home:
	mkdir -p $(SAMSON_HOME)
	mkdir -p $(SAMSON_WORKING)
	mkdir -p /var/log/samson
	mkdir -p ${LOGSERVER_LOG_DIR}
	chown -R $(SAMSON_OWNER):$(SAMSON_OWNER) $(SAMSON_HOME) $(SAMSON_WORKING) /var/log/samson ${LOGSERVER_LOG_DIR}

help:
	less doc/makefile_targets

#vim: noexpandtab


test_makefile:
	make install_release
	make install_debug
	make install_release_all
	make install_debug_all
	make install coverage

# Verify that any new/changed source code is "compliant"
code_lint:
	for file in $$(git status --porcelain -s | grep -v \?\? | awk '{print $$2}'); \
	do \
		extension=$$(basename $${file} | sed 's/.*\.//'); \
		case "$$extension" in \
			c|h|cpp|hpp)  \
				$(CPPLINT) $${file} ; \
				;;  \
			makefile)  \
				;;  \
			*)  \
				echo "Lint for .$$extension files not currently supported" ;  \
				;;  \
		esac \
	done

# Uncrusfity all files in libs and apps directory
uncrustify:
	uncrustify -c scripts/uncrustify.cfg --replace --no-backup $$(find libs -name *.cpp)
	uncrustify -c scripts/uncrustify.cfg --replace --no-backup $$(find libs -name *.h)
	uncrustify -c scripts/uncrustify.cfg --replace --no-backup $$(find apps -name *.cpp)
	uncrustify -c scripts/uncrustify.cfg --replace --no-backup $$(find apps -name *.h)
	rm -f $$(find . -name *.uncrustify)



# Handy targets for mac compilation
di_47:
	CC=gcc-mp-4.7 CXX=g++-mp-4.7 make di

di_clang:
	CC=clang CXX=clang++ make di
