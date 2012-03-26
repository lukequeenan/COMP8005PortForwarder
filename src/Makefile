CC=gcc
CFLAGS=-W -Wall -g
TFLAGS=-lpthread -pthread
PFLAGS=-lpcap
BUILDDIR=/bin
VPATH=src
SRC=/src

PROJECT_NAME=portForwarder.out

project: forward.o eventLoop.o clientHash.o serverHash.o ruleHash.o sharedLibrary.o
	$(CC) $(CFLAGS) $(TFLAGS) $(PFLAGS) -lnet forward.o eventLoop.o clientHash.o serverHash.o ruleHash.o sharedLibrary.o main.c -o $(PROJECT_NAME)

clean:
	rm -f *.o *.bak *.out ex

mapHash: uthash.h ruleHash.c ruleHash.h clientHash.c clientHash.h serverHash.c serverHash.h
	$(CC) $(CFLAGS) -O -c clientHash.c serverHash.c ruleHash.c

eventLoop: sharedLibrary.o eventLoop.c eventLoop.h
	$(CC) $(CFLAGS) $(TFLAGS) $(PFLAGS) -O -c eventLoop.c

forward: sharedLibrary.o forward.c forward.h
	$(CC) $(CFLAGS) -O -c forward.c

sharedLibrary: sharedLibrary.c sharedLibrary.h
	$(CC) $(CFLAGS) -O -c sharedLibrary.c
