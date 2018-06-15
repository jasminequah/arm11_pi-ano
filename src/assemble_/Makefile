CC	= gcc
CFLAGS	= -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

all: assemble.o ioutils.o utilities.o parse.o
	$(CC) assemble.o ioutils.o utilities.o parse.o -o ../assemble

assemble.o: assemble.c
	$(CC) $(CFLAGS) assemble.c -c -o assemble.o

parse.o: parse.c
	$(CC) $(CFLAGS) parse.c -c -o parse.o

ioutils.o: ioutils.c 
	$(CC) $(CFLAGS) ioutils.c -c -o ioutils.o

utilities.o: utilities.c
	$(CC) $(CFLAGS) utilities.c -c -o utilities.o

clean:
	-rm *.o ../assemble
