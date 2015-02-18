CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -O1
LIBS = -lm

.PHONY: all clean

all: test eratosthenes atkin

test: test.c bitvector.c
	$(CC) $(CFLAGS) -o $@ test.c bitvector.c

eratosthenes: eratosthenes.c bitvector.c
	$(CC) $(CFLAGS) -o $@ eratosthenes.c bitvector.c

atkin: atkin.c bitvector.c
	$(CC) $(CFLAGS) -o $@ atkin.c bitvector.c $(LIBS)

clean:
	# object files
	rm -f *.o
	# binary files
	rm -f atkin bitvector eratosthenes test
