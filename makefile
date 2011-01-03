all:
	./scripts/prepareBuild
	make -C build

package: all
	make -C build package 

install: all
	sudo make -C build install

distribute: install
	./scripts/samsonDistribute

i: install

d:
	samsonLocal

clean:
	make -C build clean

xcode:	
	./scripts/prepareXcode

reset_svn:
	sudo rm -Rf build/*
	sudo rm -Rf xcode_proj

uninstall:
	sudo rm -f /usr/local/bin/samson* 
	sudo rm -Rf /usr/local/include/samson	

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

memory:
	sudo sysctl -w kernel.shmmax=134217728

