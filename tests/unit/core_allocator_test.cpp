/*
 *  core_allocator_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector get_allocator
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

template<typename T>
struct stateful_allocator
{
  using value_type = T;
  int id;

  constexpr stateful_allocator(int i = 0) : id(i) {}

  template<typename U>
  constexpr stateful_allocator(const stateful_allocator<U>& other) noexcept : id(other.id)
  {}

  constexpr T* allocate(std::size_t n) { return std::allocator<T>{}.allocate(n); }

  constexpr void deallocate(T* p, std::size_t n) { std::allocator<T>{}.deallocate(p, n); }

  friend constexpr bool operator==(const stateful_allocator& a, const stateful_allocator& b) { return a.id == b.id; }
};

constexpr bool test_constexpr_allocator()
{
  using alloc_t = stateful_allocator<std::uint64_t>;
  alloc_t a1(42);

  bitvector<std::uint64_t, alloc_t> bv(10, a1);

  if (bv.get_allocator() != a1)
    return false;
  if (bv.get_allocator().id != 42)
    return false;

  return true;
}

static_assert(test_constexpr_allocator());

// Observe allocation requests without attempting to obtain enormous storage.
struct allocation_probe
{
  using value_type = std::uint64_t;
  std::size_t* requested;
  bool* allocated;

  value_type* allocate(std::size_t n)
  {
    *requested = n;
    *allocated = true;
    throw std::bad_alloc{};
  }

  void deallocate(value_type*, std::size_t) noexcept {}
  bool operator==(allocation_probe const&) const = default;
};

struct oversized_word_range
{
  std::size_t count;
  bool* iterated;

  std::size_t size() const { return count; }
  std::uint64_t* begin() const
  {
    *iterated = true;
    throw std::logic_error("oversized range must not be iterated");
  }
  std::uint64_t* end() const
  {
    *iterated = true;
    throw std::logic_error("oversized range must not be iterated");
  }
};

bool test_storage_size_overflow()
{
  using bv_type = bitvector<std::uint64_t, allocation_probe>;
  constexpr auto max_bits = std::numeric_limits<bv_type::size_type>::max();
  constexpr auto width = bv_type::bits_per_word;
  // Cover the word boundary immediately below the overflowing addition too.
  constexpr auto last_full_word = max_bits - max_bits % width;

  for (auto bits : {last_full_word, last_full_word + 1, max_bits})
  {
    bv_type::size_type requested = 0;
    bool allocated = false;
    try
    {
      bv_type bv(bits, allocation_probe{&requested, &allocated});
      return false;
    }
    catch (std::bad_alloc const&)
    {}

    auto expected = max_bits / width;
    if (bits != last_full_word)
      ++expected;
    if (!allocated || requested != expected)
      return false;
  }

  bv_type::size_type requested = 0;
  bool allocated = false;
  bool iterated = false;
  oversized_word_range source{max_bits / width + 1, &iterated};
  try
  {
    bv_type bv(source, allocation_probe{&requested, &allocated});
    return false;
  }
  catch (std::length_error const&)
  {}
  catch (std::exception const&)
  {
    return false;
  }
  return !allocated && !iterated;
}

int main()
{
  if (!test_storage_size_overflow())
    return 1;

  using alloc_t = stateful_allocator<std::uint64_t>;

  alloc_t a1(101);
  bitvector<std::uint64_t, alloc_t> bv1(100, a1);

  if (bv1.get_allocator().id != 101)
    return 1;

  bitvector<std::uint64_t, alloc_t> bv2 = bv1;
  if (bv2.get_allocator().id != 101)
    return 1;

  auto a2 = bv1.get_allocator();
  if (a2 != a1)
    return 1;

  bitvector<std::uint64_t, std::allocator<std::uint64_t>> default_bv(100);
  default_bv.set(0);
  if (!default_bv.get(0))
    return 1;

  std::array<std::byte, 1024> buffer;
  std::pmr::monotonic_buffer_resource resource(buffer.data(), buffer.size());
  std::pmr::polymorphic_allocator<std::uint64_t> pmr_alloc(&resource);

  bitvector<std::uint64_t, std::pmr::polymorphic_allocator<std::uint64_t>> pmr_bv(100, pmr_alloc);
  pmr_bv.set_all();

  if (pmr_bv.count() != 100)
    return 1;
  if (pmr_bv.get_allocator() != pmr_alloc)
    return 1;
  if (pmr_bv.get_allocator().resource() != &resource)
    return 1;

  alloc_t a3(202);
  bitvector<std::uint64_t, alloc_t> uninit_bv(50, uninitialized, a3);
  if (uninit_bv.size() != 50)
    return 1;
  if (uninit_bv.get_allocator().id != 202)
    return 1;

  return 0;
}
