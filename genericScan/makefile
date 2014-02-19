all:scan run

CC=g++

scan:scan.o genscan.o
	@g++ scan.c genscan.c -o scan

run:scan
	@./scan 10

clean:
	@/bin/rm -rf *.o
