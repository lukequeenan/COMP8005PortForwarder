CC=gcc
CFLAGS=-W -Wall -g
BUILDDIR=/bin
VPATH=src
SRC=/src

PROJECT_NAME=portForwarder.out

project: sharedLibrary.o
	$(CC) $(CFLAGS) sharedLibrary.o main.c -o $(PROJECT_NAME)

clean:
	rm -f *.o *.bak *.out ex

sharedLibrary: sharedLibrary.c sharedLibrary.h
	$(CC) $(CFLAGS) -O -c sharedLibrary.c


