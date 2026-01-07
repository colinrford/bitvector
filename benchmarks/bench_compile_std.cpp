/*
 *  bench_compile_std.cpp – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 */

import std;

#ifndef TEST_SIZE
#define TEST_SIZE 100000
#endif

constexpr std::uint64_t compile_time_size = TEST_SIZE;
constexpr auto run_sieve = []() {
  std::bitset<compile_time_size> bv{};
  std::uint64_t firstprime = 2;
  auto prime = firstprime;
  std::uint64_t running_count = 0;
  while (prime + 1 < compile_time_size - 1)
  {
    auto primecopy = prime;
    while (prime * primecopy < compile_time_size - 1)
    {
      bv.set(prime * primecopy);
      primecopy++;
    }
    while (prime + 1 < (compile_time_size - 1) && bv[++prime]);
    running_count++;
  }
  return running_count;
};

constexpr std::uint64_t get_expected_count(std::uint64_t n)
{
  if (n == 100'000)
    return 9592;
  if (n == 262'144)
    return 23000;
  if (n == 524'288)
    return 43389;
  if (n == 1'000'000)
    return 78498;
  if (n == 2'000'000)
    return 148933;
  if (n == 3'000'000)
    return 216816;
  if (n == 4'000'000)
    return 283146;
  if (n == 5'000'000)
    return 348513;
  if (n == 6'000'000)
    return 412849;
  if (n == 7'000'000)
    return 476648;
  if (n == 8'000'000)
    return 539777;
  if (n == 9'000'000)
    return 602489;
  if (n == 10'000'000)
    return 664579;
  return 0;
}

// Verification
constexpr auto verify_count = []() {
  auto actual = run_sieve();
  auto expected = get_expected_count(compile_time_size);
  if (expected != 0)
  {
    if (actual != expected)
      throw "Prime count mismatch";
  }
  return actual;
};

static_assert(verify_count() > 0);

int main() {}
