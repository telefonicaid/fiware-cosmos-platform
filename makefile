
default:
	svn up
	make dd


# ------------------------------------------------
# Global install scripts
# ------------------------------------------------

i: install

install: release 
	sudo make -C BUILD_RELEASE install
	make -C modules

clean:
	make -C BUILD_DEBUG clean
	make -C BUILD_RELEASE clean	

# ------------------------------------------------
# RELEASE Version
# ------------------------------------------------

release:
	mkdir BUILD_RELEASE || true
	cd BUILD_RELEASE; cmake .. -DCMAKE_BUILD_TYPE=RELEASE
	make -C BUILD_RELEASE #-j 7

# ------------------------------------------------
# DEBUG Version
# ------------------------------------------------

debug:
	mkdir BUILD_DEBUG || true
	cd BUILD_DEBUG; cmake .. -DCMAKE_BUILD_TYPE=DEBUG
	make -j 4 -C BUILD_DEBUG 

# ------------------------------------------------                                                                                                           
# DEBUG Version                                                                                                                                             
# ------------------------------------------------

debug_coverage:
	mkdir BUILD_DEBUG_COVERAGE || true
	cd BUILD_DEBUG_COVERAGE; cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DCOVERAGE=True
	make -C BUILD_DEBUG_COVERAGE -j8

# ------------------------------------------------                                                                                                           
# Testing
# ------------------------------------------------

test: ctest itest

ctest: debug
	make test -C BUILD_DEBUG

test_local_processes:
	./scripts/test_local_processes

package: release
	make -C BUILD_RELEASE package 

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
	./scripts/samsonDistribute

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

module:
	make -C build/apps/samsonModuleParser


qt:
	make -C build/apps/delilahQt

clean_qt:
	make -C build/apps/delilahQt clean


v: debug
	valgrind -v  --leak-check=full --track-origins=yes --show-reachable=yes  ./BUILD_DEBUG/apps/samsonLocal/samsonLocal  2> output_valgrind_samsonLocal


coverage: debug
	cd BUILD_DEBUG ; ../scripts/samsonCoverage

clear_ipcs:
	echo $(for i in `ipcs -m | grep $USER | awk '{print $2}'`; do ipcrm -m $i; done)


