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
	mkdir build;cd build;samsonModuleParser ../module ../ Module
	mkdir build;cd build;cmake .. -DCMAKE_BUILD_TYPE=DEBUG;cd ..

prepare_release:
	mkdir build;cd build;samsonModuleParser ../module ../ Module
	mkdir build;cd build;cmake .. -DCMAKE_BUILD_TYPE=RELEASE;cd ..

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
