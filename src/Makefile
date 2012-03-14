CC=gcc
CFLAGS=-W -Wall -g
BUILDDIR=/bin
VPATH=src
SRC=/src

PROJECT_NAME=portForwarder.out

project: mapHash.o sharedLibrary.o
	$(CC) $(CFLAGS) mapHash.o sharedLibrary.o main.c -o $(PROJECT_NAME)

clean:
	rm -f *.o *.bak *.out ex

sharedLibrary: sharedLibrary.c sharedLibrary.h
	$(CC) $(CFLAGS) -O -c sharedLibrary.c

mapHash: uthash.h mapHash.c mapHash.h
	$(CC) $(CFLAGS) -O -c mapHash.c