GCC = gcc
C99 = -std=c99
WER = -Werror -Wall -Wextra
O2 = -O2
O3 = -O3
TAR = tar
ZC = -zcvf
LM = -lm

all: test eratosthenes atkin

test: test.o bitvector.o
	$(GCC) $(WER) $(C99) test.c bitvector.c -o testBv

eratosthenes: eratosthenes.o
	$(GCC) $(WER) $(O3) $(C99) eratosthenes.c bitvector.c -o eratosthenesBv

atkin: atkin.o
	$(GCC) $(WER) $(O3) $(C99) atkin.c bitvector.c -o atkinBv $(LM)

tar: 
	$(TAR) $(ZC) bitvector.tar.gz bitvector.c bitvector.h test.c eratosthenes.c atkin.c eratosthenes.txt atkin.txt Makefile

clean:
	rm -rf *o test eratosthenes atkin bitvector
