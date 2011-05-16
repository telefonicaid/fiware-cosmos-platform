

all:
	make compile
	sudo make install

compile: prepare
	make -C build

prepare:
	mkdir build;cd build;samsonModuleParser ../module .
	mkdir build;cd build;cmake ..;cd ..


install:
	make -C build install


reset:
	rm -Rf build

clean:
	make clean -C build
clear:
	make clean -C build
