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
 * During testing, this program was able to successfully find the
 * prime numbers between 0 and 1,000,000. It did so very quickly. My 
 * implementation seems to miss just one prime number at large N, but
 * I have not found the issue yet. 
 *
 * I checked execution time by using the time command while connected
 * to the lab machines (data in particular)
 *
 * limit = 45,000 : 
 *
 * $ time ./atkinBv > atkinBv
 *
 * real 0m0.010s
 * user 0m0.000s
 * sys  0m0.000s
 *
 *
 * limit = 1,000,000 :
 * 
 * $ time ./atkinBv > atkinBv
 * 
 * real 0m0.153s
 * user 0m0.040s
 * sys  0m0.040s
 *
 * By removing print statements, my program runs at 0m0.035s!!!
 *
 * Note: I kept some of the comments in from the Java implementation.
 * Note 2: I have not tried using atkin.c for a limit > 1,000,000.
 */

#include "bitvector.h"
#include <math.h>

static int limit = 1000000;

int main() 
{
  int i, n, x, y, primecount;

  int limitSqrt = (int) sqrt((double) limit);
  Bitvector* sieve = make_bv(limit + ONE);
  primecount = 0;

  // initialize results array
  for (i = 0; i < limit; i++)
    clear(sieve, i);
  // the sieve works only for integers > 3, so 
  // set these trivially to their proper values
  clear(sieve, 0);
  clear(sieve, 1);
  set(sieve, 2);
  set(sieve, 3);

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
        if (get(sieve, n))
          clear(sieve, n);
        else
          set(sieve, n);
      }
      // second quadratic using m = 12 and r in R2 = {r : 7}
      n = (3 * x * x) + (y * y);
      if (n <= limit && (n % 12 == 7)) 
      {
        if (get(sieve, n))
          clear(sieve, n);
        else
          set(sieve, n);
      }
      // third quadratic using m = 12 and r in R3 = {r : 11}
      n = (3 * x * x) - (y * y);
      if (x > y && n <= limit && (n % 12 == 11)) 
      {
        if (get(sieve, n))
          clear(sieve, n);
        else
          set(sieve, n);
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
    if (get(sieve, n)) 
    {
      x = n * n;
      for (i = x; i <= limit; i += x) 
        clear(sieve, i);
    }
  for (i = 0; i <= limit; i++) // j = 0
    if (get(sieve, i)) 
    {
      //printf("%d\t", i);
      //if (++j % 10 == 0) 
        //printf("\n");
      primecount++;
    }

  printf("\nNumber of primes: %d\n", primecount);

  free_bv(sieve);

  return 0;
}
