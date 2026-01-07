/*
 *  sieve_test.cpp - correctness test for segmented sieve
 */

import std;
import lam.bitvector;

using lam::bitvec::bitvector;
using lam::bitvec::sieve_of_eratosthenes;
using lam::bitvec::sieve_of_eratosthenes_segmented;

void check(std::uint64_t n)
{
  auto simple = sieve_of_eratosthenes(n);
  auto segmented = sieve_of_eratosthenes_segmented(n);

  if (simple.size() != segmented.size())
  {
    std::println(std::cerr, "Size mismatch for N={}: {} vs {}", n, simple.size(), segmented.size());
    std::exit(1);
  }

  if (simple.count() != segmented.count())
  {
    std::println(std::cerr, "Count mismatch for N={}: {} vs {}", n, simple.count(), segmented.count());
    // Find first mismatch
    for (std::size_t i = 0; i < n; ++i)
    {
      if (simple[i] != segmented[i])
      {
        std::println(std::cerr, "Mismatch at index {}: simple={} segmented={}", i, bool(simple[i]), bool(segmented[i]));
        break;
      }
    }
    std::exit(1);
  }
}

int main()
{
  std::println("Testing segmented sieve correctness...");

  check(100);
  check(1000);
  check(10000);
  check(100000); // Trigger fallback
  check(300000); // Trigger segmented path

  auto small_seg = sieve_of_eratosthenes_segmented(1000, 128);
  auto simple = sieve_of_eratosthenes(1000);
  if (simple.count() != small_seg.count())
  {
    std::println(std::cerr, "Mismatch with small segment size!");
    std::exit(1);
  }

  // Test large
  check(10'000'000);

  std::println("All tests passed.");
  return 0;
}
