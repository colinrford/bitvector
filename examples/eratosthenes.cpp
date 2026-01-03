/*
 *  eratosthenes.cpp – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 */

import std;
import lam.bitvector;

using lam::bitvec::bitvector;

int main()
{
  constexpr std::uint64_t firstprime = 2;
  constexpr std::uint64_t num_bits = 10000000;
  
  // Use a IIFE (Immediately Invoked Lambda) to compute prime count at compile-time/runtime
  // Note: For very large sizes, this might exceed constexpr step limits if run at compile-time.
  // We'll run it at runtime for now to verify logic first.
  
  auto run_sieve = []() {
    bitvector<> bv(num_bits); // Default allocator (constexpr friendly)
    auto prime = firstprime;
    std::uint64_t running_count = 0;
    
    // Clear initial composite numbers (even numbers > 2) - optimization, skipping for naive implementation matching original
    
    while (prime + 1 < num_bits - 1)
    {
      auto primecopy = prime;
      while (prime * primecopy < num_bits - 1)
      {
        bv.set(prime * primecopy);
        primecopy++;
      }
      while (prime + 1 < (num_bits - 1) && bv[++prime]);
      running_count++;
    }
    return running_count;
  };

  // Compile-time Sieve Execution
  constexpr std::uint64_t compile_time_size = 100'000;
  constexpr auto run_sieve_ct = [](std::size_t N) {
    bitvector<> bv(N);
    auto prime = firstprime;
    std::uint64_t running_count = 0;
    while (prime + 1 < N - 1)
    {
      auto primecopy = prime;
      while (prime * primecopy < N - 1)
      {
        bv.set(prime * primecopy);
        primecopy++;
      }
      while (prime + 1 < (N - 1) && bv[++prime]);
      running_count++;
    }
    return running_count;
  };
  
  constexpr auto ct_count = run_sieve_ct(compile_time_size);
  static_assert(ct_count == 9592); // Known prime count for 100,000
  
  // Runtime Execution
  std::println("Compile-time verification passed (N={}).", compile_time_size);

  // Runtime execution for the large size
  std::println("Calculating primes up to {}...", num_bits);
  auto count = run_sieve();
  
  std::println("There are {} primes in the range 0 - {}\n",
               count,
               num_bits);
}
