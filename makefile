debug:
	mkdir BUILD_DEBUG || TRUE
	cd BUILD_DEBUG; cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DCOVERAGE=True
	make -C BUILD_DEBUG

release:
	mkdir BUILD_RELEASE || TRUE
	cd BUILD_RELEASE; cmake .. -DCMAKE_BUILD_TYPE=RELEASE
	make -C BUILD_RELEASE

test: ctest itest


itest:	test_local_processes


ctest:
	make test -C BUILD_DEBUG

test_local_processes:
	./scripts/test_local_processes


package: release
	make -C BUILD_RELEASE package 

install: release
	sudo make -C BUILD_RELEASE install

distribute: install
	./scripts/samsonDistribute

i: install


clean:
	make -C BUILD_DEBUG clean

xcode:	
	./scripts/prepareXcode

reset_svn:
	sudo rm -Rf BUILD_DEBUG/*
	sudo rm -Rf BUILD_RELEASE/*
	sudo rm -Rf xcode_proj
	

uninstall:
	sudo rm -f /usr/local/bin/samson* 
	sudo rm -Rf /usr/local/include/samson

reset:
	sudo rm -Rf BUILD_DEBUG/*
	sudo rm -Rf BUILD_RELEASE/*
	sudo rm -Rf xcode_proj
	sudo rm -f libs/common/samson.pb.*
	sudo rm -f libs/data/data.pb.*
	sudo rm -Rf /usr/local/include/samson
	sudo rm -f testing/module_test/Module.*

module:
	make -C build/apps/samsonModuleParser


qt:
	make -C build/apps/delilahQt

clean_qt:
	make -C build/apps/delilahQt clean

memory_128:
	sudo sysctl -w kernel.shmmax=134217728

memory_256::
	sudo sysctl -w kernel.shmmax=268435456

v_sl:
	valgrind -v  --leak-check=full --track-origins=yes --show-reachable=yes  samsonLocal  2> output_valgrind_samsonLocal

coverage: debug
	cd BUILD_DEBUG ; ../scripts/samsonCoverage
