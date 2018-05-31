CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

emulate: emulate.o utilities.o ioutils.o
	$(CC) emulate.o utilities.o ioutils.o -o emulate

emulate.o: emulate.c definitions.h utilities.h
	$(CC) $(CFLAGS) emulate.c -c -o emulate.o

utilities.o: utilities.c definitions.h
	$(CC) $(CFLAGS) utilities.c -c -o utilities.o

ioutils.o: ioutils.c definitions.h
	$(CC) $(CFLAGS) ioutils.c -c -o ioutils.o
clean:
	-rm *.o emulate

.PHONY: clean
