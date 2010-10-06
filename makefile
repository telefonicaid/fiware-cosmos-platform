all:
	./scripts/build
	sudo ./scripts/build install	

install:
	sudo ./scripts/build install

clean:
	make -C build clean

xcode:	
	mkdir -p xcode_proj
	cd xcode_proj;cmake -G Xcode ..

reset:
	rm -Rf build
	rm -Rf xcode_proj
