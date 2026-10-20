/*
 *  bitvector-algorithms.cppm – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  lam.bitvector is a c++ module
 */

export module lam.bitvector:algorithms;

import std;
import :core;

namespace lam::bitvec
{
namespace stdr = std::ranges;
namespace stdv = std::views;

namespace internals
{
constexpr std::size_t uint_sqrt(std::size_t n) noexcept
{
  if (n < 2)
    return n;

  std::size_t result = 0;
  std::size_t low = 1;
  std::size_t high = n / 2 + 1;

  while (low <= high)
  {
    auto mid = low + (high - low) / 2;

    if (mid <= n / mid)
    {
      result = mid;
      low = mid + 1;
    }
    else
      high = mid - 1;
  }

  return result;
}
} // end namespace internals

// Returns n bits representing [0, n); bit i is set iff i is prime.
export
template<word_c Word = std::uint64_t,
         word_allocator_c<Word> Allocator = std::allocator<Word>>
constexpr bitvector<Word, Allocator>
sieve_of_eratosthenes(std::size_t n, Allocator const& alloc = Allocator())
{
  bitvector<Word, Allocator> bv(n, alloc);
  if (n < 2)
    return bv;
  for (std::size_t p = 2; p * p < n; ++p)
    if (!bv[p])
      for (std::size_t i = p * p; i < n; i += p)
        bv[i] = true;

  bv.flip_all();
  bv.clear(0);
  bv.clear(1);

  return bv;
}

// segment size in bits; default is 256 KiB of storage
constexpr std::size_t DEFAULT_SEGMENT_SIZE = 256ULL * 1024 * 8;
constexpr std::size_t SEGMENTED_SIEVE_THRESHOLD = 100000;

// Returns n bits representing [0, n); bit i is set iff i is prime.
export
template<word_c Word = std::uint64_t,
         word_allocator_c<Word> Allocator = std::allocator<Word>>
constexpr bitvector<Word, Allocator>
sieve_of_eratosthenes_segmented(std::size_t n,
                                std::size_t segment_size = DEFAULT_SEGMENT_SIZE,
                                Allocator const& alloc = Allocator())
{
  if (segment_size == 0)
    throw std::invalid_argument("sieve segment must be positive");

  if (n <= segment_size || n <= SEGMENTED_SIEVE_THRESHOLD)
    return sieve_of_eratosthenes<Word>(n, alloc);

  using size_type = typename bitvector<Word, Allocator>::size_type;

  auto sqrt_n = internals::uint_sqrt(n);
  auto base_bv = sieve_of_eratosthenes<Word>(sqrt_n + 1, alloc);

  std::vector<size_type> base_primes;
  base_primes.reserve(sqrt_n / 2);

  stdr::copy_if(stdv::iota(size_type{2}, sqrt_n + 1),
                std::back_inserter(base_primes),
                [&base_bv](size_type p) -> bool {
                  return static_cast<bool>(base_bv[p]);
                }
              );

  bitvector<Word, Allocator> result(n, alloc);

  size_type segment_count = n / segment_size + (n % segment_size != 0);

  for (size_type segment : stdv::iota(size_type{0}, segment_count))
  {
    size_type low = segment * segment_size;
    size_type high = low + std::min(segment_size, n - low);

    for (size_type p : base_primes)
    {
      // first multiple of p >= low
      size_type start = ((low + p - 1) / p) * p;
      if (start < p * p)
        start = p * p; // never mark p itself

      if (start >= high)
        continue;

      for (size_type i = start; i < high; i += p)
        result[i] = true;
    }
  }

  result.flip_all();
  result.clear(0);
  result.clear(1);
  return result;
}
// Returns n bits representing [0, n); bit i is set iff i is prime.
export
template<word_c Word = std::uint64_t,
         word_allocator_c<Word> Allocator = std::allocator<Word>>
constexpr bitvector<Word, Allocator>
sieve_of_atkin(std::size_t n, Allocator const& alloc = Allocator())
{
  bitvector<Word, Allocator> siever(n, alloc);
  if (n < 2)
    return siever;

  if (n > 2)
    siever.set(2);
  if (n > 3)
    siever.set(3);

  using size_type = typename bitvector<Word, Allocator>::size_type;
  size_type limit_sqrt = internals::uint_sqrt(n);

  constexpr size_type ATKIN_MODULUS = 12;
  constexpr size_type ATKIN_REM_1 = 1;
  constexpr size_type ATKIN_REM_5 = 5;
  constexpr size_type ATKIN_REM_7 = 7;
  constexpr size_type ATKIN_REM_11 = 11;

  for (size_type x = 1; x <= limit_sqrt; x++)
  {
    for (size_type y = 1; y <= limit_sqrt; y++)
    {
      size_type k = (4 * x * x) + (y * y);
      if (k < n && (k % ATKIN_MODULUS == ATKIN_REM_1
                  || k % ATKIN_MODULUS == ATKIN_REM_5))
        siever[k].flip();

      k = (3 * x * x) + (y * y);
      if (k < n && (k % ATKIN_MODULUS == ATKIN_REM_7))
        siever[k].flip();

      if (x > y)
      {
        k = (3 * x * x) - (y * y);
        if (k < n && (k % ATKIN_MODULUS == ATKIN_REM_11))
          siever[k].flip();
      }
    }
  }

  constexpr size_type ATKIN_SIEVE_START = 5;
  for (size_type i = ATKIN_SIEVE_START; i <= limit_sqrt; i++)
  {
    if (siever[i])
    {
      size_type sq = i * i;
      for (size_type j = sq; j < n; j += sq)
        siever[j] = false;
    }
  }

  return siever;
}
} // namespace lam::bitvec
