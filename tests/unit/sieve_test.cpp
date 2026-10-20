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

  // Check the prime-flag contract, including small exclusive bounds.
  for (std::size_t n = 0; n <= 10; ++n)
  {
    auto simple = sieve_of_eratosthenes<std::uint32_t>(n);
    auto segmented = sieve_of_eratosthenes_segmented<std::uint32_t>(n, 3);
    auto atkin = lam::bitvec::sieve_of_atkin<std::uint32_t>(n);
    if (simple.size() != n || segmented.size() != n || atkin.size() != n)
      return 1;
    for (std::size_t i = 0; i < n; ++i)
    {
      bool prime = i == 2 || i == 3 || i == 5 || i == 7;
      if (simple[i] != prime || segmented[i] != prime || atkin[i] != prime)
        return 1;
    }
  }

  for (std::size_t n : {std::size_t{0}, std::size_t{100001}})
  {
    try
    {
      auto invalid = sieve_of_eratosthenes_segmented(n, 0);
      return 1;
    }
    catch (std::invalid_argument const&)
    {}
  }

  // Force segmentation with a nondefault word type and a partial final segment.
  auto words32 = sieve_of_eratosthenes_segmented<std::uint32_t>(100003, 127);
  auto reference32 = sieve_of_eratosthenes<std::uint32_t>(100003);
  if (!std::ranges::equal(words32.words(), reference32.words()))
    return 1;

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

  check(10'000'000);

  std::println("All tests passed.");
  return 0;
}
