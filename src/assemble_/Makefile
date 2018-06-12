CC	= gcc
CFLAGS	= -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

assemble: assemble.o ioutils.o utilities.o parse.o
	$(CC) assemble.o ioutils.o utilities.o parse.o -o ../assemble

assemble.o: assemble.c parse.h ioutils.h utilities.h definitions.h
	$(CC) $(CFLAGS) assemble.c -c -o assemble.o

parse.o: parse.c ioutils.h utilities.h definitions.h
	$(CC) $(CFLAGS) parse.c -c -o parse.o

ioutils.o: ioutils.c utilities.h definitions.h parse.h
	$(CC) $(CFLAGS) ioutils.c -c -o ioutils.o

utilities.o: utilities.c definitions.h
	$(CC) $(CFLAGS) utilities.c -c -o utilities.o

clean:
	-rm *.o ../assemble
