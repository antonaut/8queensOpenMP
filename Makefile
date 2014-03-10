CC=gcc
CCFLAGS=-g



all: hw3.o
	$(CC) -o hw3 $(CCFLAGS) hw3.c -fopenmp

clean:
	touch ./hw3
	touch ./hw3.o
	rm ./hw3
	rm ./hw3.o

check-syntax:
	$(CC) -o /dev/null -S $(CHK_SOURCES)
