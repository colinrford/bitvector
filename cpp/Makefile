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
	$(GCC) $(WER) $(C99) eratosthenes.c bitvector.cpp -o eratosthenesBv

atkin: atkin.o
	$(GCC) $(WER) $(O3) $(C99) atkin.c bitvector.cpp -o atkinBv $(LM)

tar: 
	$(TAR) $(ZC) bitvector.tar.gz bitvector.cpp bitvector.h test.c eratosthenes.c atkin.c eratosthenes.txt atkin.txt Makefile

clean:
	rm -rf *o test eratosthenes atkin bitvector
