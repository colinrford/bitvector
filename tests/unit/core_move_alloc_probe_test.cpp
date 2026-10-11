/*
 *  core_move_alloc_probe_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  move-assign between unequal, non-propagating allocators must free through the owner
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

static std::map<void*, int>& owner_ledger()
{
  static std::map<void*, int> m;
  return m;
}
static int& foreign_free_count()
{
  static int n = 0;
  return n;
}

// records each block's owner and counts frees made through another instance
template<typename T>
struct tracking_allocator
{
  using value_type = T;
  int id = 0;

  tracking_allocator() = default;
  explicit tracking_allocator(int i) : id(i) {}
  template<typename U>
  tracking_allocator(const tracking_allocator<U>& other) noexcept : id(other.id) {}

  using propagate_on_container_copy_assignment = std::false_type;
  using propagate_on_container_move_assignment = std::false_type;
  using propagate_on_container_swap            = std::false_type;
  using is_always_equal                        = std::false_type;

  T* allocate(std::size_t n)
  {
    T* p = std::allocator<T>{}.allocate(n);
    owner_ledger()[p] = id;
    return p;
  }
  void deallocate(T* p, std::size_t n) noexcept
  {
    auto it = owner_ledger().find(p);
    if (it != owner_ledger().end() && it->second != id)
    {
      ++foreign_free_count();
      std::println(std::cerr,
                   "  FOREIGN FREE: allocator #{} freed memory owned by allocator #{}",
                   id, it->second);
    }
    owner_ledger().erase(p);
    std::allocator<T>{}.deallocate(p, n);
  }

  friend bool operator==(const tracking_allocator& a, const tracking_allocator& b)
  { return a.id == b.id; }
};

int main()
{
  using alloc_t = tracking_allocator<std::uint64_t>;

  // unequal allocators: must copy, not steal
  {
    alloc_t a1(1);
    alloc_t a2(2);
    bitvector<std::uint64_t, alloc_t> a(256, a1);
    bitvector<std::uint64_t, alloc_t> b(256, a2);
    b.set(7);
    b.set(200);

    a = std::move(b);

    if (!a.get(7) || !a.get(200))
    {
      std::println(std::cerr, "  data not transferred by move-assign");
      return 1;
    }
  }

  // equal allocators: stealing is allowed
  {
    alloc_t a3(9);
    alloc_t a4(9);
    bitvector<std::uint64_t, alloc_t> c(128, a3);
    bitvector<std::uint64_t, alloc_t> d(128, a4);
    d.set_all();
    c = std::move(d);
    if (c.count() != 128)
    {
      std::println(std::cerr, "  equal-allocator move lost data");
      return 1;
    }
  }

  if (foreign_free_count() != 0)
  {
    std::println(std::cerr,
                 "FAIL: {} cross-allocator free(s) detected in move-assignment",
                 foreign_free_count());
    return 1;
  }

  std::println("ok: no cross-allocator frees");
  return 0;
}
