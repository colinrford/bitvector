/*
 *  sieve_atkin_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  sieve_of_atkin against trial division, sieve_of_eratosthenes and known pi(n)
 */

import std;
import lam.bitvector;

using lam::bitvec::sieve_of_atkin;
using lam::bitvec::sieve_of_eratosthenes;

namespace
{

bool is_prime_trial(std::uint64_t k)
{
  if (k < 2)
    return false;
  for (std::uint64_t d = 2; d * d <= k; ++d)
    if (k % d == 0)
      return false;
  return true;
}

bool check_against_trial(std::uint64_t n)
{
  auto atkin = sieve_of_atkin(n);

  if (atkin.size() != n + 1)
  {
    std::println(std::cerr, "atkin size for n={} was {}, expected {}", n, atkin.size(), n + 1);
    return false;
  }

  for (std::uint64_t i = 0; i <= n; ++i)
  {
    bool atkin_prime = atkin[i];
    bool truth = is_prime_trial(i);
    if (atkin_prime != truth)
    {
      std::println(std::cerr,
                   "atkin(n={}) wrong at i={}: atkin says prime={}, truth={}",
                   n, i, atkin_prime, truth);
      return false;
    }
  }
  return true;
}

bool check_against_eratosthenes(std::uint64_t n)
{
  auto atkin = sieve_of_atkin(n);
  auto erato = sieve_of_eratosthenes(n + 1); // n + 1 so both cover [0, n]

  for (std::uint64_t i = 0; i <= n; ++i)
  {
    bool atkin_prime = atkin[i];
    bool erato_prime = (i >= 2) && !erato[i];
    if (atkin_prime != erato_prime)
    {
      std::println(std::cerr,
                   "atkin vs eratosthenes disagree at i={} (n={}): atkin={}, erato={}",
                   i, n, atkin_prime, erato_prime);
      return false;
    }
  }
  return true;
}

} // namespace

int main()
{
  std::println("Testing sieve_of_atkin correctness...");

  {
    constexpr std::array known_primes_under_50{2u, 3u, 5u, 7u, 11u, 13u, 17u, 19u,
                                               23u, 29u, 31u, 37u, 41u, 43u, 47u};
    auto atkin = sieve_of_atkin(49);
    for (std::uint64_t i = 0; i <= 49; ++i)
    {
      bool expected = std::ranges::find(known_primes_under_50, i) != known_primes_under_50.end();
      if (bool(atkin[i]) != expected)
      {
        std::println(std::cerr, "hardcoded check failed at i={}: atkin={}, expected={}",
                     i, bool(atkin[i]), expected);
        return 1;
      }
    }
  }

  // includes n around perfect squares, where the sqrt loop bound matters
  for (std::uint64_t n : {0u, 1u, 2u, 3u, 4u, 5u, 23u, 24u, 25u, 26u, 48u, 49u, 50u,
                          99u, 100u, 120u, 121u, 122u, 168u, 169u, 170u, 1000u,
                          5000u, 10000u, 65535u, 65536u})
  {
    if (!check_against_trial(n))
      return 1;
    if (!check_against_eratosthenes(n))
      return 1;
  }

  struct pi_case { std::uint64_t n, pi; };
  for (auto [n, pi] : {pi_case{10, 4}, pi_case{100, 25}, pi_case{1000, 168},
                       pi_case{10000, 1229}, pi_case{100000, 9592}})
  {
    auto atkin = sieve_of_atkin(n);
    if (atkin.count() != pi)
    {
      std::println(std::cerr, "pi({}) from atkin = {}, expected {}", n, atkin.count(), pi);
      return 1;
    }
  }

  std::println("All atkin tests passed.");
  return 0;
}
