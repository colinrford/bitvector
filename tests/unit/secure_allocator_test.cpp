/*
 *  secure_allocator_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  proof-of-concept for secure scrubbing allocator
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

// zeroes memory on deallocate; proof of concept, not for real use
template<typename T>
struct secure_scrub_allocator
{
  using value_type = T;

  constexpr secure_scrub_allocator() noexcept {}
  template<typename U>
  constexpr secure_scrub_allocator(const secure_scrub_allocator<U>&) noexcept
  {}

  constexpr T* allocate(std::size_t n) { return std::allocator<T>{}.allocate(n); }

  constexpr void deallocate(T* p, std::size_t n) noexcept
  {
    // volatile keeps the zeroing from being optimized out
    if (!std::is_constant_evaluated())
    {
      volatile unsigned char* vp = reinterpret_cast<volatile unsigned char*>(p);
      for (std::size_t i = 0; i < n * sizeof(T); ++i)
        vp[i] = 0;
    }
    std::allocator<T>{}.deallocate(p, n);
  }

  friend constexpr bool operator==(const secure_scrub_allocator&, const secure_scrub_allocator&) { return true; }
};

constexpr bool test_constexpr_secure()
{
  bitvector<std::uint64_t, secure_scrub_allocator<std::uint64_t>> bv(64);
  bv.set(0);
  if (!bv[0])
    return false;
  bv.flip_all();
  if (bv[0])
    return false;
  return true;
}

int main()
{
  static_assert(word_allocator_c<secure_scrub_allocator<std::uint64_t>, std::uint64_t>);

  static_assert(test_constexpr_secure());

  {
    bitvector<std::uint64_t, secure_scrub_allocator<std::uint64_t>> secret_bv(100);

    secret_bv.set_all();
    if (!secret_bv.all())
      return 1;
  }

  return 0;
}
