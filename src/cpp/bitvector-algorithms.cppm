/*
 *  bitvector-algorithms.cppm – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  lam.bitvector is a c++ module
 */

export module lam.bitvector:algorithms;

import std;
import :core;

namespace lam::bitvec
{
// Returns a bitvector where the i-th bit is set if i is composite.
// 0 and 1 are ignored (left as 0) but are not prime.
export template<bitvector_allocator_c Allocator = std::allocator<std::uint64_t>>
constexpr bitvector<Allocator> sieve_of_eratosthenes(std::size_t n, const Allocator& alloc = Allocator())
{
  bitvector<Allocator> bv(n, alloc); // Initialized to 0 (all assumed prime)
  // We will only mark composites starting from 2 * 2.
  if (n < 2)
    return bv;
  for (std::size_t p = 2; p * p < n; ++p)
    if (!bv[p]) // if p is prime
      for (std::size_t i = p * p; i < n; i += p)
        bv.set(i);
  return bv;
}

// Segmented Sieve of Eratosthenes for better cache locality (large N)
// segment_size: number of bits (integers) to process at once.
// Default is 256KB * 8 bits = 2M bits (fits in typical L2 cache).
constexpr std::size_t DEFAULT_SEGMENT_SIZE = 256ULL * 1024 * 8;
constexpr std::size_t SEGMENTED_SIEVE_THRESHOLD = 100000;

export template<bitvector_allocator_c Allocator = std::allocator<std::uint64_t>>
bitvector<Allocator> sieve_of_eratosthenes_segmented(std::size_t n, std::size_t segment_size = DEFAULT_SEGMENT_SIZE,
                                                     const Allocator& alloc = Allocator())
{
  if (n <= segment_size || n <= SEGMENTED_SIEVE_THRESHOLD)
    return sieve_of_eratosthenes(n, alloc);

  // 1. Compute base primes up to sqrt(N)
  auto sqrt_n = static_cast<std::size_t>(std::sqrt(static_cast<double>(n)));
  auto base_bv = sieve_of_eratosthenes(sqrt_n + 1, alloc);

  std::vector<std::size_t> base_primes;
  // Rough upper bound for pi(x) is x / (ln x - 1)
  if (sqrt_n > 2)
    base_primes.reserve(sqrt_n / 2); // simplistic reservation

  for (std::size_t i = 2; i <= sqrt_n; ++i)
    if (!base_bv[i])
      base_primes.push_back(i);

  // 2. Prepare result
  bitvector<Allocator> result(n, alloc);

  // 3. Segmented Sieve
  for (std::size_t low = 0; low < n; low += segment_size)
  {
    std::size_t high = std::min(low + segment_size, n);

    for (std::size_t p : base_primes)
    {
      // First multiple of p >= low
      std::size_t start = ((low + p - 1) / p) * p;
      if (start < p * p)
        start = p * p; // Start at p^2 to avoid marking p itself
      if (start < low)
        start += p; // Ensure we are in or after current segment (rounding fix)

      if (start >= high)
        continue;

      for (std::size_t i = start; i < high; i += p)
        result.set(i);
    }
  }
  return result;
}
// Sieve of Atkin
// Returns a bitvector where the i-th bit is set if i is PRIME.
// Note: This semantics (1=Prime) differs from sieve_of_eratosthenes (1=Composite).
export template<bitvector_allocator_c Allocator = std::allocator<std::uint64_t>>
constexpr bitvector<Allocator> sieve_of_atkin(std::size_t n, const Allocator& alloc = Allocator())
{
  bitvector<Allocator> siever(n + 1, alloc);
  if (n < 2) return siever;

  siever.set(2);
  if (n > 2) siever.set(3);

  auto limitSqrt = static_cast<std::size_t>(std::sqrt(static_cast<double>(n)));

  constexpr std::size_t ATKIN_MODULUS = 12;
  constexpr std::size_t ATKIN_REM_1 = 1;
  constexpr std::size_t ATKIN_REM_5 = 5;
  constexpr std::size_t ATKIN_REM_7 = 7;
  constexpr std::size_t ATKIN_REM_11 = 11;

  for (std::size_t x = 1; x <= limitSqrt; x++) 
  {
    for (std::size_t y = 1; y <= limitSqrt; y++) 
    {
      std::size_t k = (4 * x * x) + (y * y);
      if (k <= n && (k % ATKIN_MODULUS == ATKIN_REM_1 || k % ATKIN_MODULUS == ATKIN_REM_5)) 
        siever.toggle(k);

      k = (3 * x * x) + (y * y);
      if (k <= n && (k % ATKIN_MODULUS == ATKIN_REM_7)) 
        siever.toggle(k);

      if (x > y)
      {
        k = (3 * x * x) - (y * y);
        if (k <= n && (k % ATKIN_MODULUS == ATKIN_REM_11)) 
          siever.toggle(k);
      }
    }
  }

  constexpr std::size_t ATKIN_SIEVE_START = 5;
  for (std::size_t i = ATKIN_SIEVE_START; i <= limitSqrt; i++) 
  {
    if (siever[i]) 
    {
      std::size_t sq = i * i;
      for (std::size_t j = sq; j <= n; j += sq) 
        siever.clear(j);
    }
  }

  return siever;
}
} // namespace lam::bitvec
