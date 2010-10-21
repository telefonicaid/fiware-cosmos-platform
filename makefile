all:
	make prepare
	make -C build

prepare:	
	./scripts/prepareBuild

install:
	sudo make -C build install

mi:
	make
	make install

i:
	make install

d:
	samsonDemo -ncurses

clean:
	make -C build clean

xcode:	
	./scripts/prepareXcode

reset:
	sudo rm -Rf build/*
	sudo rm -Rf xcode_proj
	sudo rm -f modules/example/Module.*
	sudo rm -f libs/network/samson.pb.*
	sudo rm -f libs/core/data.pb.*


module:
	make -C build/apps/samsonModuleParser


core:
	make -C build/libs/module
	sudo make install -C build/libs/module
	

example:
	touch modules/example/Module
	make -C build/modules/example

qt:
	make -C build/apps/delilahQt

clean_qt:
	make -C build/apps/delilahQt clean