all:
	g++ main.cpp list.cpp hashtable.cpp hashfuncs.cpp crc32.o tests.cpp -o Table.out -mavx2 -march=native -O1
	./Table.out

debug:
	g++ -g main.cpp list.cpp hashtable.cpp hashfuncs.cpp tests.cpp -o Table.out -mavx2 -march=native -O1
	gdb ./Table.out