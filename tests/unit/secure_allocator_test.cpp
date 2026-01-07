/*
 *  secure_allocator_test.cpp – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  proof-of-concept for secure scrubbing allocator
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

// Secure Allocator Proof-of-Concept, don't use this
// This allocator scrubs memory (fills with zeros) upon deallocation
// to prevent sensitive data from lingering in RAM.
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
    // SCRUBBING
    // Use volatile cast to prevent Dead Store Elimination (compiler optimizing away the write)
    if (!std::is_constant_evaluated())
    {
      volatile unsigned char* vp = reinterpret_cast<volatile unsigned char*>(p);
      for (std::size_t i = 0; i < n * sizeof(T); ++i)
        vp[i] = 0;
    }
    // For demonstration, we just rely on standard deallocate after scrubbing
    std::allocator<T>{}.deallocate(p, n);
  }

  friend constexpr bool operator==(const secure_scrub_allocator&, const secure_scrub_allocator&) { return true; }
};

constexpr bool test_constexpr_secure()
{
  // Verify basic operations work at compile-time with this allocator
  bitvector<secure_scrub_allocator<std::uint64_t>> bv(64);
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
  // 1. Verify it satisfies the Bitvector Allocator Concept
  static_assert(bitvector_allocator_c<secure_scrub_allocator<std::uint64_t>>);

  // 2. Verify Constexpr Usage
  static_assert(test_constexpr_secure());

  // 3. Usage Test (Runtime)
  {
    // Create a "secret" bitvector using the secure allocator
    bitvector<secure_scrub_allocator<std::uint64_t>> secret_bv(100);

    secret_bv.set_all();
    if (!secret_bv.all())
      return 1;

    // ... operations ...

    // Destructor will strictly call deallocate(), which runs the scrub loop.
  }

  return 0;
}
