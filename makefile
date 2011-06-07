debug:
	mkdir BUILD_DEBUG || true
	cd BUILD_DEBUG; cmake .. -DCMAKE_BUILD_TYPE=DEBUG
	make -j 7 -C BUILD_DEBUG 

debug_coverage:
	mkdir BUILD_DEBUG_COVERAGE || true
	cd BUILD_DEBUG_COVERAGE; cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DCOVERAGE=True
	make -C BUILD_DEBUG_COVERAGE -j8

release:
	mkdir BUILD_RELEASE || true
	cd BUILD_RELEASE; cmake .. -DCMAKE_BUILD_TYPE=RELEASE
	make -C BUILD_RELEASE -j 7

install: release
	sudo make -C BUILD_RELEASE install

install_samson: install
	make -C modules


test: ctest itest


itest:	test_local_processes


ctest: debug
	make test -C BUILD_DEBUG

test_local_processes:
	./scripts/test_local_processes


package: release
	make -C BUILD_RELEASE package 


di:	debuginstall

debuginstall: debug
	sudo make -C BUILD_DEBUG install

distribute: install
	./scripts/samsonDistribute

d:	distribute

dd:	debugdistribute

debugdistribute: debuginstall
	./scripts/samsonDistribute

testdistribute: install
	./scripts/samsonTestDistributer

i: install


clean:
	make -C BUILD_DEBUG clean

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

memory_128:
	sudo sysctl -w kernel.shmmax=134217728

memory_256::
	sudo sysctl -w kernel.shmmax=268435456

v: debug
	valgrind -v  --leak-check=full --track-origins=yes --show-reachable=yes  ./BUILD_DEBUG/apps/samsonLocal/samsonLocal  2> output_valgrind_samsonLocal

sl: debug
	./BUILD_DEBUG/apps/samsonLocal/samsonLocal

coverage: debug
	cd BUILD_DEBUG ; ../scripts/samsonCoverage

cm:
	echo $(for i in `ipcs -m | grep $USER | awk '{print $2}'`; do ipcrm -m $i; done)


s: sync_ss9

sync_ss9:
	rsync -r libs              samson@samson09:/home/samson/samson_sync
	rsync -r apps              samson@samson09:/home/samson/samson_sync
	rsync -r scripts           samson@samson09:/home/samson/samson_sync
	rsync -r packaging 	   samson@samson09:/home/samson/samson_sync
	rsync makefile             samson@samson09:/home/samson/samson_sync
	rsync CMakeLists.txt       samson@samson09:/home/samson/samson_sync

rsl: sync_ss9
	ssh samson@samson09 make release -C /home/samson/samson_sync
	ssh samson@samson09 /home/samson/samson_sync/BUILD_RELEASE/apps/samsonLocal
