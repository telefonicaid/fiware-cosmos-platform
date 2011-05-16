

all:
	make prepare
	make -C build
	sudo make install -C build

prepare:
	mkdir build;cd build;cmake ..;cd ..


xcode:
	mkdir xcodeProj;cd xcodeProj;cmake .. -G Xcode;make;cd ..

install:
	sudo make -C build install

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
