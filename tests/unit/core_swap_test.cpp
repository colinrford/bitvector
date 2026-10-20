/*
 *  core_swap_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector core swap
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

template<bool Propagate, bool AlwaysEqual = false>
struct swap_allocator
{
  using value_type = std::uint64_t;
  using propagate_on_container_swap = std::bool_constant<Propagate>;
  using is_always_equal = std::bool_constant<AlwaysEqual>;

  int domain;
  int metadata;
  inline static std::map<value_type*, int> owners;
  inline static bool foreign_free = false;
  inline static bool forbid_allocation = false;

  value_type* allocate(std::size_t n)
  {
    if (forbid_allocation)
      throw std::bad_alloc{};
    auto* p = std::allocator<value_type>{}.allocate(n);
    try
    {
      owners.emplace(p, domain);
    }
    catch (...)
    {
      std::allocator<value_type>{}.deallocate(p, n);
      throw;
    }
    return p;
  }

  void deallocate(value_type* p, std::size_t n) noexcept
  {
    auto it = owners.find(p);
    if (it == owners.end())
      foreign_free = true;
    else
    {
      if constexpr (!AlwaysEqual)
        if (it->second != domain)
          foreign_free = true;
      owners.erase(it);
    }
    std::allocator<value_type>{}.deallocate(p, n);
  }

  bool operator==(swap_allocator const& other) const
  { return AlwaysEqual || domain == other.domain; }
};

template<bool Propagate, bool AlwaysEqual = false>
bool test_allocator_swap()
{
  using alloc = swap_allocator<Propagate, AlwaysEqual>;
  using bv = bitvector<std::uint64_t, alloc>;
  static_assert(std::is_nothrow_swappable_v<bv> == (Propagate || AlwaysEqual));
  using std::swap;

  for (bool equal_domains : {false, true})
    for (std::size_t b_bits : {std::size_t{0}, std::size_t{50}})
    {
      alloc::foreign_free = false;
      {
        int b_domain = equal_domains ? 1 : 2;
        bv a(100, alloc{1, 11});
        a.set(1);
        a.set(99);
        bv b(b_bits, alloc{b_domain, 22});
        if (b_bits != 0)
          b.set(7);
        auto* a_data = a.words().data();
        auto* b_data = b.words().data();

        // Self-swap must preserve both contents and allocator state.
        alloc::forbid_allocation = true;
        swap(a, a);
        bool threw = false;
        try
        {
          swap(a, b);
        }
        catch (std::invalid_argument const&)
        {
          threw = true;
        }
        alloc::forbid_allocation = false;

        bool should_throw = !Propagate && !AlwaysEqual && !equal_domains;
        if (threw != should_throw)
          return false;

        if (threw)
        {
          if (a.size() != 100 || a.word_count() != 2
              || a.words().data() != a_data || a.count() != 2
              || !a.get(1) || !a.get(99)
              || b.size() != b_bits || b.words().data() != b_data
              || b.count() != (b_bits == 0 ? 0u : 1u)
              || (b_bits != 0 && !b.get(7))
              || a.get_allocator().domain != 1 || a.get_allocator().metadata != 11
              || b.get_allocator().domain != b_domain || b.get_allocator().metadata != 22)
            return false;
        }
        else
        {
          if (a.size() != b_bits || a.words().data() != b_data
              || a.count() != (b_bits == 0 ? 0u : 1u)
              || (b_bits != 0 && !a.get(7))
              || b.size() != 100 || b.word_count() != 2
              || b.words().data() != a_data || b.count() != 2
              || !b.get(1) || !b.get(99)
              || a.get_allocator().domain != (Propagate ? b_domain : 1)
              || a.get_allocator().metadata != (Propagate ? 22 : 11)
              || b.get_allocator().domain != (Propagate ? 1 : b_domain)
              || b.get_allocator().metadata != (Propagate ? 11 : 22))
            return false;
        }
      }
      if (alloc::foreign_free || !alloc::owners.empty())
        return false;
    }
  return true;
}

int main()
{
  if (!test_allocator_swap<true>() || !test_allocator_swap<false>()
      || !test_allocator_swap<false, true>())
  {
    std::println(std::cerr, "allocator-aware swap tests failed");
    return 1;
  }

  bitvector<> a(100);
  a.set_all();

  bitvector<> b(50);
  b.reset();

  using std::swap;
  swap(a, b);

  if (a.size() != 50)
    return 1;
  if (a.count() != 0)
    return 1;

  if (b.size() != 100)
    return 1;
  if (b.count() != 100)
    return 1;

  return 0;
}
