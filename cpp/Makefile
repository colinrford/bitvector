CC = g++
CFLAGS = -Wall -Wextra -Werror -O1
LIBS = -lm

.PHONY: all clean

all: test eratosthenes atkin

test: test.cpp bitvector.cpp
	$(CC) $(CFLAGS) -o $@ test.cpp bitvector.cpp

eratosthenes: eratosthenes.cpp bitvector.cpp
	$(CC) $(CFLAGS) -o $@ eratosthenes.cpp bitvector.cpp

atkin: atkin.cpp bitvector.cpp
	$(CC) $(CFLAGS) -o $@ atkin.cpp bitvector.cpp $(LIBS)

clean:
	# object files
	rm -f *.o
	# binary files
	rm -f atkin bitvector eratosthenes test
