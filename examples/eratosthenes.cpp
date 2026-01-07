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
  
  // Helper to count primes from sieve result
  auto count_primes = [](const auto& bv, std::uint64_t limit) {
    if (limit < 2) return std::uint64_t{0};
    // Sieve marks composites. Primes in [2, limit) = Total[2, limit) - Composites
    // Total numbers in [2, limit) is (limit - 1) - 2 + 1 = limit - 2
    // bv.count() returns number of set bits (composites)
    // assuming bv has size >= limit
    return (limit - 2) - bv.count();
  };

  auto run_sieve = [&]() {
    auto bv = lam::bitvec::sieve_of_eratosthenes(num_bits);
    return count_primes(bv, num_bits);
  };

  // Compile-time Sieve Execution
  constexpr std::uint64_t compile_time_size = 100'000;
  constexpr auto run_sieve_ct = [](std::size_t N) {
    auto bv = lam::bitvec::sieve_of_eratosthenes(N);
    // Cannot use lambda helper easily in constexpr if not structured right, 
    // but calculating inline is easy.
    if (N < 2) return std::uint64_t{0};
    return static_cast<std::uint64_t>((N - 2) - bv.count());
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
