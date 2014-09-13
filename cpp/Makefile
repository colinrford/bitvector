GCC = g++
C99 = -std=c99
WER = -Werror -Wall -Wextra
O2 = -O2
O3 = -O3
TAR = tar
ZC = -zcvf
LM = -lm

all: test eratosthenes atkin

test: test.o bitvector.o
	$(GCC) $(WER) test.cpp bitvector.cpp -o testBv

eratosthenes: eratosthenes.o
	$(GCC) $(WER) $(O3) eratosthenes.cpp bitvector.cpp -o eratosthenesBv

atkin: atkin.o
	$(GCC) $(WER) $(O3) atkin.cpp bitvector.cpp -o atkinBv $(LM)

tar: 
	$(TAR) $(ZC) bitvector.tar.gz bitvector.cpp bitvector.h test.cpp eratosthenes.cpp atkin.cpp eratosthenes.txt atkin.txt Makefile

clean:
	rm -rf *o test eratosthenes atkin bitvector
