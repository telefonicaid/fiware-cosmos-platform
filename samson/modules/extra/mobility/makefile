MODULE=$(lastword $(subst /, ,$(CURDIR)))
ifndef SAMSON_HOME
SAMSON_HOME=/opt/samson
endif

all: prepare_release release

debug: prepare_debug
	make -C BUILD_DEBUG

release: prepare_release
	make -C BUILD_RELEASE

prepare_debug:
	mkdir -p BUILD_DEBUG
	cd BUILD_DEBUG; cmake ..  -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME)
	cd ..

prepare_release:
	mkdir -p BUILD_RELEASE
	cd BUILD_RELEASE; cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=$(SAMSON_HOME)
	cd ..

xcode:
	mkdir xcodeProj;cd xcodeProj;cmake .. -G Xcode;make;cd ..

install: install_release 
install_release: prepare_release
	make -C BUILD_RELEASE install
install_debug: prepare_debug
	make -C BUILD_DEBUG install

t:
	touch module

reset:
	rm -Rf BUILD_RELEASE
	rm -Rf BUILD_DEBUG
	rm -f Module.*
	rm -Rf xcodeProj
	rm -f samson/modules/*/*_base.h
	rm -Rf $(SAMSON_HOME)/include/samson/modules/$(MODULE)
	rm -f $(SAMSON_HOME)/modules/lib$(MODULE).so


clean: clean_release
clean_release:
	make clean -C BUILD_RELEASE
clean_debug:
	make clean -C BUILD_DEBUG

eclipse:
	../../scripts/prepareModulesEclipse
