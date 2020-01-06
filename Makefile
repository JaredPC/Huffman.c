CC = gcc
CFLAGS = -Wall -ansi -pedantic -g
MAIN = hencode hdecode
OBJS = hencode.o htable.o
all : $(MAIN)

hencode: hencode.o htable.o
	$(CC) -o hencode hencode.o htable.o

hdecode: hdecode.o htable.o
	$(CC) -o hdecode hdecode.o htable.o

hencode.o : hencode.c hencode.h
	$(CC) $(CFLAGS) -c -o hencode.o hencode.c 

hdecode.o : hdecode.c hdecode.h
	$(CC) $(CFLAGS) -c -o hdecode.o hdecode.c 

htable.o : htable.c htable.h
	$(CC) $(CFLAGS) -c -o htable.o htable.c

clean :
	rm *.o $(MAIN) core