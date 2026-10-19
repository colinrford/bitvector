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

struct assignment_allocation_state
{
  std::map<std::uint64_t*, int> owners;
  bool fail = false;
  bool foreign_free = false;
  std::size_t allocations = 0;
};

template<bool Propagate, bool MovePropagate = false>
struct assignment_allocator
{
  using value_type = std::uint64_t;
  using propagate_on_container_copy_assignment = std::bool_constant<Propagate>;
  using propagate_on_container_move_assignment = std::bool_constant<MovePropagate>;
  using is_always_equal = std::false_type;

  assignment_allocation_state* state;
  int domain;
  int metadata;

  value_type* allocate(std::size_t n)
  {
    if (state->fail)
      throw std::bad_alloc{};
    auto* p = std::allocator<value_type>{}.allocate(n);
    try
    {
      state->owners.emplace(p, domain);
    }
    catch (...)
    {
      std::allocator<value_type>{}.deallocate(p, n);
      throw;
    }
    ++state->allocations;
    return p;
  }

  void deallocate(value_type* p, std::size_t n) noexcept
  {
    auto it = state->owners.find(p);
    if (it == state->owners.end() || it->second != domain)
      state->foreign_free = true;
    if (it != state->owners.end())
      state->owners.erase(it);
    std::allocator<value_type>{}.deallocate(p, n);
  }

  // Metadata may differ even when allocations are interchangeable.
  bool operator==(assignment_allocator const& other) const
  { return state == other.state && domain == other.domain; }
};

template<bool Propagate>
bool test_copy_allocator()
{
  using alloc = assignment_allocator<Propagate>;
  using bv = bitvector<std::uint64_t, alloc>;

  for (bool equal : {false, true})
    for (std::size_t dest_bits : {std::size_t{10}, std::size_t{130}})
      for (bool fail : {false, true})
      {
        assignment_allocation_state state;
        {
          bv source(100, alloc{&state, equal ? 1 : 2, 22});
          source.set(1);
          source.set(99);
          bv dest(dest_bits, alloc{&state, 1, 11});
          dest.set(3);
          auto* old_data = dest.words().data();
          auto allocations = state.allocations;
          bool needs_allocation = dest.word_count() != source.word_count()
                               || (Propagate && !equal);
          state.fail = fail;
          bool threw = false;
          try
          {
            if (&(dest = source) != &dest)
              return false;
          }
          catch (std::bad_alloc const&)
          {
            threw = true;
          }

          if (threw != (fail && needs_allocation))
            return false;
          if (threw)
          {
            if (dest.size() != dest_bits || dest.words().data() != old_data
                || dest.count() != 1 || !dest.get(3)
                || dest.get_allocator().domain != 1
                || dest.get_allocator().metadata != 11)
              return false;
          }
          else
          {
            if (dest.size() != 100 || dest.count() != 2
                || !dest.get(1) || !dest.get(99)
                || dest.get_allocator().domain != (Propagate ? source.get_allocator().domain : 1)
                || dest.get_allocator().metadata != (Propagate ? 22 : 11))
              return false;
            if (state.allocations != allocations + (needs_allocation ? 1u : 0u))
              return false;
            dest.clear(99);
          }
          if (source.size() != 100 || source.count() != 2
              || !source.get(1) || !source.get(99))
            return false;
        }
        if (state.foreign_free || !state.owners.empty())
          return false;
      }
  return true;
}

template<bool Propagate>
bool test_move_allocator()
{
  // Opposite copy/move policies catch accidental delegation to copy assignment.
  using alloc = assignment_allocator<!Propagate, Propagate>;
  using bv = bitvector<std::uint64_t, alloc>;
  static_assert(std::is_nothrow_move_assignable_v<bv> == Propagate);

  for (bool equal : {false, true})
    for (std::size_t dest_bits : {std::size_t{10}, std::size_t{130}})
      for (bool fail : {false, true})
      {
        assignment_allocation_state state;
        {
          int source_domain = equal ? 1 : 2;
          bv source(100, alloc{&state, source_domain, 22});
          source.set(1);
          source.set(99);
          bv dest(dest_bits, alloc{&state, 1, 11});
          dest.set(3);
          auto* source_data = source.words().data();
          auto* old_data = dest.words().data();
          auto allocations = state.allocations;
          bool steals = Propagate || equal;
          bool needs_allocation = !steals && dest.word_count() != source.word_count();
          state.fail = fail;
          bool threw = false;
          try
          {
            if (&(dest = std::move(source)) != &dest)
              return false;
          }
          catch (std::bad_alloc const&)
          {
            threw = true;
          }

          if (threw != (fail && needs_allocation))
            return false;
          if (threw)
          {
            if (dest.size() != dest_bits || dest.words().data() != old_data
                || dest.count() != 1 || !dest.get(3)
                || dest.get_allocator().domain != 1
                || dest.get_allocator().metadata != 11)
              return false;
          }
          else
          {
            if (dest.size() != 100 || dest.count() != 2
                || !dest.get(1) || !dest.get(99)
                || dest.get_allocator().domain != (Propagate ? source_domain : 1)
                || dest.get_allocator().metadata != (Propagate ? 22 : 11)
                || state.allocations != allocations + (needs_allocation ? 1u : 0u))
              return false;
            if (steals && dest.words().data() != source_data)
              return false;
            if (!steals && !needs_allocation && dest.words().data() != old_data)
              return false;
            dest.clear(99);
          }

          if (steals)
          {
            if (source.size() != 0 || !source.words().empty())
              return false;
          }
          else if (source.size() != 100 || source.words().data() != source_data
                   || source.count() != 2 || !source.get(1) || !source.get(99))
            return false;
        }
        if (state.foreign_free || !state.owners.empty())
          return false;
      }
  return true;
}

// Every instance can free every allocation, even when its metadata differs.
struct always_equal_move_allocator
{
  using value_type = std::uint64_t;
  using propagate_on_container_move_assignment = std::false_type;
  using is_always_equal = std::true_type;
  int metadata;
  inline static bool fail = false;

  value_type* allocate(std::size_t n)
  {
    if (fail)
      throw std::bad_alloc{};
    return std::allocator<value_type>{}.allocate(n);
  }
  void deallocate(value_type* p, std::size_t n) noexcept
  { std::allocator<value_type>{}.deallocate(p, n); }
  bool operator==(always_equal_move_allocator const&) const { return true; }
};

bool test_always_equal_move_assignment()
{
  using bv = bitvector<std::uint64_t, always_equal_move_allocator>;
  static_assert(std::is_nothrow_move_assignable_v<bv>);
  bv source(100, always_equal_move_allocator{22});
  source.set(99);
  bv dest(10, always_equal_move_allocator{11});
  auto* source_data = source.words().data();
  always_equal_move_allocator::fail = true;
  dest = std::move(source);
  always_equal_move_allocator::fail = false;
  return dest.size() == 100 && dest.count() == 1 && dest.get(99)
      && dest.words().data() == source_data && dest.get_allocator().metadata == 11
      && source.size() == 0 && source.words().empty();
}

bool test_pmr_move_assignment()
{
  std::pmr::unsynchronized_pool_resource source_resource;
  std::pmr::unsynchronized_pool_resource dest_resource;
  using alloc = std::pmr::polymorphic_allocator<std::uint64_t>;
  using bv = bitvector<std::uint64_t, alloc>;
  static_assert(!std::is_nothrow_move_assignable_v<bv>);
  for (bool equal : {false, true})
    for (std::size_t dest_bits : {std::size_t{10}, std::size_t{130}})
    {
      auto* resource = equal ? &dest_resource : &source_resource;
      bv source(100, alloc{resource});
      source.set(99);
      auto* source_data = source.words().data();
      bv dest(dest_bits, alloc{&dest_resource});
      dest.set_all();
      dest = std::move(source);
      if (dest.get_allocator().resource() != &dest_resource
          || dest.size() != 100 || dest.count() != 1 || !dest.get(99))
        return false;
      dest.clear(99);
      if (equal)
      {
        if (dest.words().data() != source_data || source.size() != 0 || !source.words().empty())
          return false;
      }
      else if (source.size() != 100 || source.count() != 1 || !source.get(99))
        return false;
    }
  return true;
}

bool test_pmr_copy_assignment()
{
  std::pmr::unsynchronized_pool_resource source_resource;
  std::pmr::unsynchronized_pool_resource dest_resource;
  using alloc = std::pmr::polymorphic_allocator<std::uint64_t>;
  for (std::size_t dest_bits : {std::size_t{10}, std::size_t{130}})
  {
    bitvector<std::uint64_t, alloc> source(100, alloc{&source_resource});
    source.set(99);
    bitvector<std::uint64_t, alloc> dest(dest_bits, alloc{&dest_resource});
    dest.set_all();
    dest = source;
    if (dest.get_allocator().resource() != &dest_resource
        || dest.size() != 100 || dest.count() != 1 || !dest.get(99))
      return false;
    dest.clear(99);
    if (!source.get(99))
      return false;
  }
  return true;
}

int main()
{
  if (!test_move_allocator<false>() || !test_move_allocator<true>()
      || !test_always_equal_move_assignment() || !test_pmr_move_assignment())
  {
    std::println(std::cerr, "move assignment allocator tests failed");
    return 1;
  }

  if (!test_copy_allocator<false>() || !test_copy_allocator<true>()
      || !test_pmr_copy_assignment())
    return 1;

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
