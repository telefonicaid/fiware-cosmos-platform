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


module:
	make -C build/apps/samsonModuleParser


core:
	make -C build/libs/module
	sudo make install -C build/libs/module
	

example:
	touch modules/example/Module
	make -C build/modules/example