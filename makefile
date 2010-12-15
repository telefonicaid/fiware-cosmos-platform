all:
	make prepare
	make -C build

prepare:	
	./scripts/prepareBuild

install:
	make
	sudo make -C build install

distribute: install
	./scripts/samsonDistribute

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

reset_svn:
	sudo rm -Rf build/*
	sudo rm -Rf xcode_proj

reset:
#	sudo rm -Rf build/*
#	sudo rm -Rf xcode_proj
	sudo rm -f modules/example/Module.*
	sudo rm -f libs/network/samson.pb.*
	sudo rm -f libs/data/data.pb.*
	sudo rm -Rf /usr/local/include/samson

module:
	make -C build/apps/samsonModuleParser


core:
	make -C build/libs/module
	sudo make install -C build/libs/module
	

example:
	touch modules/example/Module
	make -C build/modules/example
	sudo make install -C build/modules/example

qt:
	make -C build/apps/delilahQt

clean_qt:
	make -C build/apps/delilahQt clean


m:
	mkdir modulesBuild;cd modulesBuild;cmake ../modules;make;cd ..
