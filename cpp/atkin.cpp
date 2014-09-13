/* Colin Ford
 *
 * The Sieve of Atkin is a more recent sieve (2004). It is claimed
 * by some to be faster than the Sieve of Eratosthenes, but (from
 * what I have read) there has not been an implementation of the
 * algorithm yet that is any faster than Eratosthenes. Theoretically,
 * the Sieve of Atkin is faster than Eratosthenes.
 *
 * This is a Sieve of Atkin implementation that I based off of
 * a Java Sieve of Atkin implementation that I found online.
 *
 * To be honest, I am not quite sure how the Sieve of Atkin works
 * (yet)
 *
 * The C implementation of the sieve of atkins is slightly (.002 s)
 * quicker than this C++ implementation, when counting primes from
 * 0 - 1 million. Have not performed further benchmarks or 
 * comparisons.
 *
 * Note: I kept some of the comments in from the Java implementation.
 *
 */

#include "bitvector.h"
#include <math.h>

static int limit = 1000000;

int main() 
{
  int i, n, x, y, primecount;

  int limitSqrt = (int) sqrt((double) limit);
  Bitvector* sieve = new Bitvector(limit + ONE);
  primecount = 0;

  // initialize results array
  for (i = 0; i < limit; i++)
    sieve -> clear(i);
  // the sieve works only for integers > 3, so 
  // set these trivially to their proper values
  sieve -> clear(0);
  sieve -> clear(1);
  sieve -> set(2);
  sieve -> set(3);

  // loop through all possible integer values for x and y
  // up to the square root of the max prime for the sieve
  // we don't need any larger values for x or y since the
  // max value for x or y will be the square root of n
  // in the quadratics
  // the theorem showed that the quadratics will produce all
  // primes that also satisfy their wheel factorizations, so
  // we can produce the value of n from the quadratic first
  // and then filter n through the wheel quadratic 
  // there may be more efficient ways to do this, but this
  // is the design in the Wikipedia article
  // loop through all integers for x and y for calculating
  // the quadratics
  for (x = 1; x <= limitSqrt; x++) 
  {
    for (y = 1; y <= limitSqrt; y++) 
    {
      // first quadratic using m = 12 and r in R1 = {r : 1, 5}
      n = (4 * x * x) + (y * y);
      if (n <= limit && (n % 12 == 1 || n % 12 == 5)) 
      {
        if (sieve -> get(n))
          sieve -> clear(n);
        else
          sieve -> set(n);
      }
      // second quadratic using m = 12 and r in R2 = {r : 7}
      n = (3 * x * x) + (y * y);
      if (n <= limit && (n % 12 == 7)) 
      {
        if (sieve -> get(n))
          sieve -> clear(n);
        else
          sieve -> set(n);
      }
      // third quadratic using m = 12 and r in R3 = {r : 11}
      n = (3 * x * x) - (y * y);
      if (x > y && n <= limit && (n % 12 == 11)) 
      {
        if (sieve -> get(n))
          sieve -> clear(n);
        else
          sieve -> set(n);
      }
      // note that R1 union R2 union R3 is the set R
      // R = {r : 1, 5, 7, 11}
      // which is all values 0 < r < 12 where r is 
      // a relative prime of 12
      // Thus all primes become candidates
    }
  }
  // remove all perfect squares since the quadratic
  // wheel factorization filter removes only some of them
  for (n = 5; n <= limitSqrt; n++) 
    if (sieve -> get(n)) 
    {
      x = n * n;
      for (i = x; i <= limit; i += x) 
        sieve -> clear(i);
    }
  for (i = 0; i <= limit; i++) // j = 0
    if (sieve -> get(i)) 
    {
      //printf("%d\t", i);
      //if (++j % 10 == 0) 
        //printf("\n");
      primecount++;
    }

  cout << "\nNumber of primes: " << primecount << endl;

  delete sieve;

  return 0;
}
