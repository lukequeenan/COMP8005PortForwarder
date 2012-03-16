CC=gcc
CFLAGS=-W -Wall -g
TFLAGS=-lpthread -pthread
BUILDDIR=/bin
VPATH=src
SRC=/src

PROJECT_NAME=portForwarder.out

project: mapHash.o sharedLibrary.o
	$(CC) $(CFLAGS) mapHash.o sharedLibrary.o main.c -o $(PROJECT_NAME)

clean:
	rm -f *.o *.bak *.out ex

mapHash: uthash.h mapHash.c mapHash.h
	$(CC) $(CFLAGS) -O -c mapHash.c

eventLoop: sharedLibrary.o eventLoop.c eventLoop.h
	$(CC) $(CFLAGS) $(TFLAG) -O -c eventLoop.c

forward: sharedLibrary.o forward.c forward.h
	$(CC) $(CFLAGS) -O -c forward.c

sharedLibrary: sharedLibrary.c sharedLibrary.h
	$(CC) $(CFLAGS) -O -c sharedLibrary.c