/*
 *  atkin.cpp – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 */

import std;
import lam.bitvector;

using lam::bitvec::bitvector;

static constexpr int limit = 1000000;

int main()
{
  try
  {
    std::println("Calculating primes up to {} using Sieve of Atkin...", limit);

    // Note: sieve_of_atkin returns a bitvector where 1 = Prime.
    auto siever = lam::bitvec::sieve_of_atkin(limit);
    std::uint64_t primecount = siever.count();
    std::println("\nNumber of primes: {}", primecount);

    return 0;
  }
  catch (const std::exception& e)
  {
    std::println("Exception caught: {}", e.what());
    return 1;
  }
  catch (...)
  {
    std::println("Unknown exception caught");
    return 1;
  }
}
