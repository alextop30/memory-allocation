CC = g++
CFLAGS = -Wall -g -std=c++11

all: allocate

blockMem.o : blockMem.cpp blockMem.h
	${CC} ${CFLAGS} -c blockMem.cpp

main.o : memory_alloc.cpp blockMem.h
	${CC} ${CFLAGS} -c memory_alloc.cpp

allocate: main.o blockMem.o
	${CC} ${CFLAGS} blockMem.o memory_alloc.o -o allocate

runf:
	make all
	echo "\nRunning with first fit algorithm\n" && ./allocate f

runb:
	make all
	echo "\nRunning with best fit algorithm\n" && ./allocate b

clean:
	rm -rf *.o
	rm allocate
