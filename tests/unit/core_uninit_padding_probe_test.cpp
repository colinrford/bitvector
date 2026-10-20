/*
 *  core_uninit_padding_probe_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  bitvector(n, uninitialized) must not leave garbage in the last word's unused bits
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

// hands out storage filled with 1s so leftover bits are deterministic
template<typename T>
struct poison_allocator
{
  using value_type = T;

  poison_allocator() = default;
  template<typename U>
  poison_allocator(const poison_allocator<U>&) noexcept {}

  T* allocate(std::size_t n)
  {
    T* p = std::allocator<T>{}.allocate(n);
    std::memset(p, 0xFF, n * sizeof(T));
    return p;
  }
  void deallocate(T* p, std::size_t n) noexcept { std::allocator<T>{}.deallocate(p, n); }

  friend bool operator==(const poison_allocator&, const poison_allocator&) { return true; }
};

int main()
{
  // 10 bits in one 64-bit word; bits 10..63 are padding
  bitvector<std::uint64_t, poison_allocator<std::uint64_t>> bv(10, uninitialized);
  for (std::size_t i = 0; i < bv.size(); ++i)
    bv.set(i, false);

  int failures = 0;
  if (bv.count() != 0)
  {
    std::println(std::cerr, "  count() = {}, expected 0", bv.count());
    ++failures;
  }
  if (bv.any())
  {
    std::println(std::cerr, "  any() = true, expected false");
    ++failures;
  }
  if (!bv.none())
  {
    std::println(std::cerr, "  none() = false, expected true");
    ++failures;
  }

  if (failures != 0)
  {
    std::println(std::cerr, "FAIL: uninitialized padding bits leak into count/any/none");
    return 1;
  }
  std::println("ok: padding bits are zero");
  return 0;
}
