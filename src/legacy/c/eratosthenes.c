/* Colin Ford
 *
 * This is a Sieve of Eratosthenes implementation that I based off of
 * another implementation I found online.
 * 
 * The Sieve of Eratosthenes is an ancient but efficient method of 
 * finding the prime numbers between 0 and some large number N.
 * 
 * The Sieve of Eratosthenes works in the following way:
 * i) Create a list of consecutive integers from 2 to N, 
 *     e.g. create a bitarray with N bits
 * ii) Let p = 2, the first prime number
 *     e.g. prime = 2
 * iii) Starting from p, count to N in multiples of p (2p, 3p, ...) 
 *     e.g. set each bit at the indices equal to the multiples of p 
 *         (or non-prime number) to 1.
 * iv) Find the next number greater than p that is not marked. If 
 *     no such number exists, stop. If it does, let p equal this
 *     number and repeat step 3.
 *     e.g. find the next bit in the array that has not been set yet.
 *         If it exists, let p equal this number and repeat step 3.
 *         Otherwise, count the bits that have not been set 
 *         (bits that = 0) are the prime numbers between 0 and N.
 * 
 * During testing, this algorithm was able to successfully find the
 * prime numbers between 0 and 46377. For some reason, the program
 * will seg fault after 46377. This is somewhat perplexing to me
 * since I am able to create and manipulate bitvectors created from
 * my bitvector library that have capacities up to 1,000,000 (I have
 * not checked larger numbers).
 *
 * I checked execution time by using the time command while connected
 * to the lab machines (data in particular) (this time is also before
 * I added the initialization loop at the beginning)
 *
 * $ time ./eratosthenesBv > eratosthenesBv
 * 
 * real 0m0.012s
 * user 0m0.000s
 * sys  0m0.000s
 *
 * By removing print statements, my program runs at 0m0.009s.
 * 
 * Note: I never compiled this program with -O, -O2 or -O3, so I am 
 *       not sure if it would have been quicker under these 
 *       circumstances.
 */

#include "bitvector.h"
#include <errno.h>
#include <stdbool.h>
#define FIRSTPRIME 2
#define NUMBITS 45000

int primecount;
void printprime(int prime);

int main(int argc, char* argv[])
{
  int i, prime, primecopy, num_bits;
  Bitvector* bv;
  
  if (argc > 1)
  {
    errno = 0;
    char* p = argv[1];
    char* end;
    const long i = strtol(p, &end, 10);
 
    const bool range_error = errno == ERANGE;
    printf("Extracted '%.*s', strtol returned %ld.", (int)(end-p), p, i);
    num_bits = i;
    if (range_error)
      printf("\n --> Range error occurred.");
  } else
    num_bits = NUMBITS;
  bv = make_bv(num_bits + ONE);
  prime = FIRSTPRIME;
  primecount = 0;

  for (i = 0; i < num_bits; i++)
    clear(bv, i);

  printprime(prime);

  while (prime + ONE < num_bits - 1)
  {
    primecopy = prime;
    while (prime * primecopy < num_bits - 1)
    {
      set(bv, prime * primecopy);
      primecopy++;
    }
    while (prime + ONE < (num_bits - 1) && get(bv, ++prime));
    printprime(prime);
    primecount++;
  }

  free_bv(bv);

  printf("There are %d primes in the range 0 - %d\n", primecount, num_bits);

  return 0;
}

void printprime(int prime)
{
  static int numfound = 0;

  if (numfound % SIZEOFBYTE == 0)
    printf("\n");

  if (prime + ONE < NUMBITS)
    printf("%d\t", prime);

  numfound++;
}
