ifndef SAMSON_HOME
SAMSON_HOME=/opt/samson
endif
ifndef SAMSON_VERSION
SAMSON_VERSION=0.6
endif
ifndef SAMSON_RELEASE
SAMSON_RELEASE=1
endif

default:
	svn up
	make dd




# ------------------------------------------------
# Prepare CMAKE
# ------------------------------------------------


prepare_release:
	mkdir BUILD_RELEASE || true
	cd BUILD_RELEASE; cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME)

prepare_debug:
	mkdir BUILD_DEBUG || true
	cd BUILD_DEBUG; cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME)


prepare: prepare_release prepare_debug

# ------------------------------------------------
# Global install scripts
# ------------------------------------------------

i: install

install: release 
	sudo make -C BUILD_RELEASE install
	make release -C modules

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

debug_coverage:
	mkdir BUILD_DEBUG_COVERAGE || true
	cd BUILD_DEBUG_COVERAGE; cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DCOVERAGE=True
	make -C BUILD_DEBUG_COVERAGE # -j8

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

di:	debuginstall

debuginstall: debug
	sudo make -C BUILD_DEBUG install
	make -C modules

distribute: install
	./scripts/samsonDistribute

d:	distribute

dd:	debugdistribute

debugdistribute: debuginstall
	./scripts/samsonDist  samson03 samson04

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
	make reset -C modules


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


coverage: debug
	cd BUILD_DEBUG ; ../scripts/samsonCoverage

clear_ipcs:
	for i in `sudo ipcs -m | grep -v key  | grep -v Memory  |  awk '{print $2}'`; do sudo ipcrm -m $i; done

set_ssm_linux:
	sudo sysctl -w kernel.shmmax=64000000

rpm: install
	scripts/samsonRpm $(SAMSON_VERSION) $(SAMSON_RELEASE)
	scripts/samsonModuleDependencies
	cd modules/cdr;                   ../../scripts/samsonModuleRpm cdr $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/example;               ../../scripts/samsonModuleRpm example $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/graph;                 ../../scripts/samsonModuleRpm graph $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/osn;                   ../../scripts/samsonModuleRpm osn $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/sna;                   ../../scripts/samsonModuleRpm sna $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/sort;                  ../../scripts/samsonModuleRpm sort $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/system;                ../../scripts/samsonModuleRpm system $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/terasort;              ../../scripts/samsonModuleRpm terasort $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/testcomparefunctions; ../../scripts/samsonModuleRpm testcomparefunctions $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/tienda;                ../../scripts/samsonModuleRpm tienda $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/txt;                   ../../scripts/samsonModuleRpm txt $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/url;                   ../../scripts/samsonModuleRpm url $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/urlbenchmark;         ../../scripts/samsonModuleRpm urlbenchmark $(SAMSON_VERSION) $(SAMSON_RELEASE)
	# cd modules/sna_light;             ../../scripts/samsonModuleRpm sna_light $(SAMSON_VERSION) $(SAMSON_RELEASE)

# currently the deb scripts require Samson be installed before 
# the package can be generated. Using SAMSON_HOME we can override
# the default install location so as to not trash a live installation
deb: install
	scripts/samsonDeb $(SAMSON_VERSION) $(SAMSON_RELEASE)
	scripts/samsonModuleDependencies
	scripts/samsonDebDev $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/cdr;                   ../../scripts/samsonModuleDeb cdr $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/example;               ../../scripts/samsonModuleDeb example $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/graph;                 ../../scripts/samsonModuleDeb graph $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/osn;                   ../../scripts/samsonModuleDeb osn $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/sna;                   ../../scripts/samsonModuleDeb sna $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/sort;                  ../../scripts/samsonModuleDeb sort $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/system;                ../../scripts/samsonModuleDeb system $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/terasort;              ../../scripts/samsonModuleDeb terasort $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/testcomparefunctions; ../../scripts/samsonModuleDeb testcomparefunctions $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/tienda;                ../../scripts/samsonModuleDeb tienda $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/txt;                   ../../scripts/samsonModuleDeb txt $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/url;                   ../../scripts/samsonModuleDeb url $(SAMSON_VERSION) $(SAMSON_RELEASE)
	cd modules/urlbenchmark;         ../../scripts/samsonModuleDeb urlbenchmark $(SAMSON_VERSION) $(SAMSON_RELEASE)

man:
	 mkdir -p /usr/local/man/man1
	 mkdir -p /usr/local/man/man7
	 help2man --name="samson controller"                 --no-info --section=1 --manual=Samson samsonController       > /usr/local/man/man1/samsonController.1
	 help2man --name="samson worker"                     --no-info --section=1 --manual=Samson samsonWorker           > /usr/local/man/man1/samsonWorker.1
	 help2man --name="samson setup"                      --no-info --section=1 --manual=Samson samsonSetup            > /usr/local/man/man1/samsonSetup.1
	 help2man --name="samson spawner"                    --no-info --section=1 --manual=Samson samsonSpawner          > /usr/local/man/man1/samsonSpawner.1
	 help2man --name="samson platform interaction shell" --no-info --section=1 --manual=Samson delilah                > /usr/local/man/man1/delilah.1
	 cp man/samson-*.7 /usr/local/man/man7

packages: install man rpm deb

.PHONY : modules
.PHONY : man

# vim: noexpandtab
