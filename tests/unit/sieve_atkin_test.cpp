/*
 *  sieve_atkin_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  sieve_of_atkin against trial division and known pi(n)
 */

import std;
import lam.bitvector;

using lam::bitvec::sieve_of_atkin;

namespace
{

constexpr bool check_constexpr_atkin()
{
  constexpr std::array primes{2u, 3u, 5u, 7u, 11u, 13u, 17u, 19u,
                              23u, 29u, 31u, 37u, 41u, 43u, 47u};
  for (std::size_t n : {0u, 1u, 2u, 3u, 4u, 5u, 25u, 26u, 49u, 50u})
  {
    auto atkin = sieve_of_atkin(n);
    if (atkin.size() != n)
      return false;
    for (std::size_t i = 0; i < n; ++i)
    {
      bool expected = std::ranges::find(primes, i) != primes.end();
      if (atkin[i] != expected)
        return false;
    }
  }
  return true;
}

static_assert(check_constexpr_atkin());

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

  if (atkin.size() != n)
  {
    std::println(std::cerr, "atkin size for n={} was {}, expected {}", n, atkin.size(), n);
    return false;
  }

  for (std::uint64_t i = 0; i < n; ++i)
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

} // namespace

int main()
{
  std::println("Testing sieve_of_atkin correctness...");

  {
    constexpr std::array known_primes_under_50{2u, 3u, 5u, 7u, 11u, 13u, 17u, 19u,
                                               23u, 29u, 31u, 37u, 41u, 43u, 47u};
    auto atkin = sieve_of_atkin(50);
    for (std::uint64_t i = 0; i < 50; ++i)
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
  }

  struct pi_case { std::uint64_t n, pi; };
  for (auto [n, pi] : {pi_case{10, 4}, pi_case{100, 25}, pi_case{1000, 168},
                       pi_case{10000, 1229}, pi_case{100000, 9592},
                       pi_case{10000000, 664579}})
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
