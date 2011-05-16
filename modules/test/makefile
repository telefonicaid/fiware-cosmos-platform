
all:
	mkdir build;cd build;cmake ..;cd ..
	make -C build
	make test -C build

reset:
	rm -Rf build
	rm -f Module.*

clean:
	make clean -C build
clear:
	make clean -C build
