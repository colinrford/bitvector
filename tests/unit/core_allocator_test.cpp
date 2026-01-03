/*
 *  core_allocator_test.cpp – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector get_allocator
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

// Simple stateful allocator for testing equality/identity
template <typename T>
struct stateful_allocator
{
  using value_type = T;
  int id;
  
  constexpr stateful_allocator(int i = 0) : id(i) {}
  
  template <typename U>
  constexpr stateful_allocator(const stateful_allocator<U>& other) noexcept : id(other.id) {}

  constexpr T* allocate(std::size_t n)
  { return std::allocator<T>{}.allocate(n); }
  
  constexpr void deallocate(T* p, std::size_t n)
  { std::allocator<T>{}.deallocate(p, n); }
  
  friend constexpr bool operator==(const stateful_allocator& a, const stateful_allocator& b)
  { return a.id == b.id; }
};

constexpr bool test_constexpr_allocator()
{
  using alloc_t = stateful_allocator<std::uint64_t>;
  alloc_t a1(42);
  
  bitvector<alloc_t> bv(10, a1);
  
  if (bv.get_allocator() != a1) 
    return false;
  if (bv.get_allocator().id != 42) 
    return false;
  
  return true;
}

static_assert(test_constexpr_allocator());

int main()
{
  using alloc_t = stateful_allocator<std::uint64_t>;
  
  // Test 1: Construction with allocator
  alloc_t a1(101);
  bitvector<alloc_t> bv1(100, a1);
  
  if (bv1.get_allocator().id != 101) 
    return 1;
  
  // Test 2: Copy constructor should copy allocator
  bitvector<alloc_t> bv2 = bv1;
  if (bv2.get_allocator().id != 101) 
    return 1;
  
  // Test 3: Verify get_allocator() returns a copy that compares equal
  auto a2 = bv1.get_allocator();
  if (a2 != a1) 
    return 1;

  // Test 4: Default Allocator
  bitvector<std::allocator<std::uint64_t>> default_bv(100);
  default_bv.set(0);
  if (!default_bv.get(0)) 
    return 1;

  // Test 5: PMR Allocator
  std::array<std::byte, 1024> buffer;
  std::pmr::monotonic_buffer_resource resource(buffer.data(), buffer.size());
  std::pmr::polymorphic_allocator<std::uint64_t> pmr_alloc(&resource);
  
  // bitvector using PMR allocator
  bitvector<std::pmr::polymorphic_allocator<std::uint64_t>> pmr_bv(100, pmr_alloc);
  pmr_bv.set_all();
  
  if (pmr_bv.count() != 100) 
    return 1;
  if (pmr_bv.get_allocator() != pmr_alloc) 
    return 1;
  if (pmr_bv.get_allocator().resource() != &resource) 
    return 1;

  // Test 6: Uninitialized construction with explicit allocator
  alloc_t a3(202);
  bitvector<alloc_t> uninit_bv(50, uninitialized, a3);
  if (uninit_bv.size() != 50) return 1;
  if (uninit_bv.get_allocator().id != 202) return 1;

  return 0;
}
