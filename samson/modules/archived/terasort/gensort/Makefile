gensort: gensort.c rand16.o rand16.h
	gcc -g -o gensort gensort.c rand16.o -lz

rand16.o: rand16.c rand16.h
	gcc -g -c rand16.c

valsort: valsort.c rand16.o rand16.h
	gcc -g -o valsort valsort.c rand16.o -lz
