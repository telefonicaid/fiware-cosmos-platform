ifndef SAMSON_HOME
SAMSON_HOME=/opt/samson
endif


all:
	make compile
	sudo make install

release: 
	make compile_release
	sudo make install

test:
	make test -C build

compile: prepare
	make -C build

compile_release: prepare_release
	make -C build

prepare:
	mkdir build;cd build;../../../BUILD_RELEASE/apps/samsonModuleParser/samsonModuleParser ../module ../ Module
	mkdir build;cd build;cmake ..  -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME);cd ..

prepare_release:
	mkdir build;cd build;../../../BUILD_RELEASE/apps/samsonModuleParser/samsonModuleParser ../module ../ Module
	mkdir build;cd build;cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME) ;cd ..

xcode:
	mkdir xcodeProj;cd xcodeProj;cmake .. -G Xcode;make;cd ..

install:
	make -C build install


mi:
	make
	make install

i:
	make install

t:
	touch module

reset:
	rm -Rf build
	rm -f Module.*
	rm -Rf xcodeProj

clean:
	make clean -C build
clear:
	make clean -C build

eclipse:
	../../scripts/prepareModulesEclipse
