all:
	make prepare
	make -C build

prepare:	
	./scripts/prepareBuild

install:
	sudo make -C build install

clean:
	make -C build clean

xcode:	
	./scripts/prepareXcode

reset:
	sudo rm -Rf build/*
	sudo rm -Rf xcode_proj
