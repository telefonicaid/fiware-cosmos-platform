ifndef SAMSON_HOME
SAMSON_HOME=/opt/samson
endif
ifndef SAMSON_WORKING
SAMSON_WORKING=/var/samson
endif
ifndef SAMSON_VERSION
SAMSON_VERSION=0.6
endif
ifndef SAMSON_RELEASE
SAMSON_RELEASE=1
endif
# Who to install samson as
ifndef SAMSON_OWNER
SAMSON_OWNER=samson
endif

DISTRO=$(shell lsb_release -is)

# List of modules that gets used to build packages. *Don't forget* to update modules/makefile as well when adding a new module
MODULES=system \
	txt \
	txt_md \
	example \
	tienda \
	cdr \
	mobility \
	mob \
	mob2 \
	passive_location \
	graph \
	sna \
	snalight \
	osn \
	url \
	urlbenchmark \
	sort \
	terasort \
	testtime \
	testcomparefunctions \
	hit \
	gg \
	page_rank \
	wiki \
	simple_mobility \
	web \
	twitter

default: release modules man

# ------------------------------------------------
# Prepare CMAKE
# ------------------------------------------------


prepare_release:
	mkdir BUILD_RELEASE || true
	cd BUILD_RELEASE; cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME)

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

install_debug: debug install_man
	sudo make -C BUILD_DEBUG install
	make  -C modules

install: release install_man
	sudo make -C BUILD_RELEASE install
	make release -C modules
	# FIXME Using sudo to install the files is a bit heavy handed
	sudo mkdir -p $(SAMSON_HOME)/share/modules/moduletemplate
	sudo cp README $(SAMSON_HOME)/share/README.txt
	sudo cp modules/moduletemplate/CMakeLists.txt $(SAMSON_HOME)/share/modules/moduletemplate
	sudo cp modules/moduletemplate/makefile $(SAMSON_HOME)/share/modules/moduletemplate
	sudo cp modules/moduletemplate/module $(SAMSON_HOME)/share/modules/moduletemplate
	sudo cp scripts/samsonModuleBootstrap $(SAMSON_HOME)/bin
	sudo cp scripts/samsonProcessesSupervise $(SAMSON_HOME)/bin
	getent group samson >/dev/null || sudo groupadd -r samson
	getent passwd samson >/dev/null || sudo useradd -r -g samson -d /opt/samson -s /sbin/nologin -c 'SAMSON account' samson
	sudo chown -R $(SAMSON_OWNER):$(SAMSON_OWNER) $(SAMSON_HOME)
	sudo mkdir -p /var/samson
	sudo chown -R $(SAMSON_OWNER):$(SAMSON_OWNER) $(SAMSON_WORKING)
	sudo cp etc/profile.d/samson.sh /etc/profile.d/samson.sh
ifeq ($(DISTRO),Ubuntu)
	sudo cp etc/init.d/samson.ubuntu /etc/init.d/samson
	sudo update-rc.d samson defaults
else
ifeq ($(DISTRO),RedHatEnterpriseServer)
	sudo cp etc/init.d/samson.redhat /etc/init.d/samson
	sudo /sbin/chkconfig --add samson
	sudo /sbin/chkconfig --level 35 samson on
else
ifeq ($(DISTRO),CentOS)
	sudo cp etc/init.d/samson.redhat /etc/init.d/samson
	sudo /sbin/chkconfig --add samson
	sudo /sbin/chkconfig --level 35 samson on
endif
endif
endif

install_man: man
	sudo cp -r BUILD_RELEASE/man $(SAMSON_HOME)/
 
clean:
	make -C BUILD_DEBUG clean
	make -C BUILD_RELEASE clean	

# ------------------------------------------------
# RELEASE Version
# ------------------------------------------------


release: prepare_release
	make -C BUILD_RELEASE #-j 7

# ------------------------------------------------
# DEBUG Version
# ------------------------------------------------

debug: prepare_debug
	make -C BUILD_DEBUG #-j 4

# ------------------------------------------------
# DEBUG Version
# ------------------------------------------------

coverage: prepare_coverage
	make -C BUILD_COVERAGE # -j8
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
	sudo make -C BUILD_DEBUG install

pdid: pdi
	./scripts/samsonDist

di:	debuginstall

debuginstall: debug
	sudo make -C BUILD_DEBUG install
	make -C modules

distribute: install
	./scripts/samsonDistribute

d:	distribute

dd:	debugdistribute

debugdistribute: debuginstall
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

uninstall:
	sudo rm -f /usr/local/bin/samson* 
	sudo rm -Rf /usr/local/include/samson

reset:
	sudo rm -Rf BUILD_DEBUG
	sudo rm -Rf BUILD_RELEASE
	sudo rm -Rf BUILD_DEBUG_COVERAGE
	sudo rm -Rf xcode_proj
	sudo rm -f libs/common/samson.pb.*
	sudo rm -f libs/data/data.pb.*
	sudo rm -Rf /usr/local/include/samson
	sudo rm -f testing/module_test/Module.*
	sudo rm -f /etc/init/samson.conf
	sudo rm -f /etc/profile.d/samson.sh
	if [ "$(SAMSON_HOME)" != "/usr/local" ]; then sudo rm -rf $(SAMSON_HOME); fi
	sudo rm -rf $(SAMSON_WORKING)
	sudo rm -rf rpm
	make reset -C modules

cleansvn: reset
	for file in `svn status | grep ? | awk '{print $$2}'`; do rm -rf $$file; done

# Remove /opt/samson to guarantee a clean build
cleanhudson:
	rm -rf /opt/samson

modules: release
	make release -C modules

modules_debug: debug
	make -C modules


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

rpm: release modules man
	rm -rf package/rpm
	scripts/samsonRpm $(SAMSON_VERSION) $(SAMSON_RELEASE)
	scripts/samsonModuleDependencies
	for module in $(MODULES); do \
		cd modules/$$module; \
		../../scripts/samsonModuleRpm $$module $(SAMSON_VERSION) $(SAMSON_RELEASE); \
		cd ../..; \
	done

publish_rpm: rpm
	rsync  -v package/rpm/*.rpm repo@samson09.hi.inet:/var/repository/redhat/6/x86_64
	ssh repo@samson09.hi.inet createrepo -q -d /var/repository/redhat/6/x86_64

# currently the deb scripts require Samson be installed before 
# the package can be generated. Using SAMSON_HOME we can override
# the default install location so as to not trash a live installation
deb: release modules man
	rm -rf package/deb
	scripts/samsonDeb $(SAMSON_VERSION) $(SAMSON_RELEASE)
	scripts/samsonDebDev $(SAMSON_VERSION) $(SAMSON_RELEASE)
	scripts/samsonDebDocs $(SAMSON_VERSION) $(SAMSON_RELEASE)
	scripts/samsonModuleDependencies
	for module in $(MODULES); do \
		cd modules/$$module; \
		../../scripts/samsonModuleDeb $$module $(SAMSON_VERSION) $(SAMSON_RELEASE); \
		cd ../..; \
	done

publish_deb: deb
	ssh repo@samson09 mkdir -p /tmp/samson-deb-$(SAMSON_RELEASE)/
	scp package/deb/* repo@samson09:/tmp/samson-deb-$(SAMSON_RELEASE)/
	ssh repo@samson09 'cd /var/repository/ubuntu/natty; for deb in `reprepro list tid | grep " samson" | cut -f 2 -d" "`; do reprepro remove tid $$deb; done'
	ssh repo@samson09 'cd /var/repository/ubuntu/natty; for deb in `ls /tmp/samson-deb-$(SAMSON_RELEASE)/samson*.deb`; do reprepro includedeb tid $$deb; done'
	ssh repo@samson09 rm -rf /tmp/samson-deb-$(SAMSON_RELEASE)/

man: release
	 mkdir -p BUILD_RELEASE/man/man1
	 mkdir -p BUILD_RELEASE/man/man7
	 help2man --name="samson controller"     --no-info --section=1 --manual=Samson ./BUILD_RELEASE/apps/samsonController/samsonController       > ./BUILD_RELEASE/man/man1/samsonController.1
	 help2man --name="samson worker"         --no-info --section=1 --manual=Samson ./BUILD_RELEASE/apps/samsonWorker/samsonWorker     > ./BUILD_RELEASE/man/man1/samsonWorker.1
	 help2man --name="samson setup"          --no-info --section=1 --manual=Samson ./BUILD_RELEASE/apps/samsonSetup/samsonSetup       > ./BUILD_RELEASE/man/man1/samsonSetup.1
	 help2man --name="samson spawner"        --no-info --section=1 --manual=Samson ./BUILD_RELEASE/apps/samsonSpawner/samsonSpawner   > ./BUILD_RELEASE/man/man1/samsonSpawner.1
	 help2man --name="samson platform interaction shell" --no-info --section=1 --manual=Samson ./BUILD_RELEASE/apps/delilah/delilah                > ./BUILD_RELEASE/man/man1/delilah.1
	 cp man/samson-*.7 ./BUILD_RELEASE/man/man7

packages: install man rpm deb

.PHONY : modules
.PHONY : man

#vim: noexpandtab
