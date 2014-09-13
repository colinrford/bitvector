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
 * The C++ implementation of the sieve of eratosthenes is slightly 
 * (.001 s) slower than the C implementation, when counting primes 
 * from 0 - 45,000. Interesting, perhaps?
 *
 */

#include "bitvector.h"
#define FIRSTPRIME 2
#define NUMBITS 45000

int primecount;
void printprime(int prime);

int main()
{
  int i, prime, primecopy, num_bits;

  num_bits = NUMBITS;
  Bitvector* bv = new Bitvector(num_bits + ONE);
  prime = FIRSTPRIME;
  primecount = 0;

  for (i = 0; i < NUMBITS; i++)
    bv -> clear(i);

  printprime(prime);

  while (prime + ONE < num_bits - 1)
  {
    primecopy = prime;
    while (prime * primecopy < num_bits - 1)
    {
      bv -> set(prime * primecopy);
      primecopy++;
    }
    while (prime + ONE < (num_bits - 1) && bv -> get(++prime));
    printprime(prime);
    primecount++;
  }

  delete bv;

  cout << "There are " << primecount << " primes in the range 0 - " << NUMBITS << endl;

  return 0;
}

void printprime(int prime)
{
  static int numfound = 0;

  if (numfound % SIZEOFBYTE == 0)
    cout << endl;

  if (prime + ONE < NUMBITS)
    cout << prime << '\t';

  numfound++;
}
