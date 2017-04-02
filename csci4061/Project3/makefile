CC=gcc
CFLAGS=-I. -lm -Wall

test: test.o mini_filesystem.o
	$(CC) -o test test.o mini_filesystem.o $(CFLAGS)


mini_filesystem.o: mini_filesystem.c mini_filesystem.h
	$(CC) -c mini_filesystem.c

test.o: test.c mini_filesystem.h
	$(CC) -c test.c
