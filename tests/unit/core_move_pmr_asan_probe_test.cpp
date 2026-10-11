/*
 *  core_move_pmr_asan_probe_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  move-assign between pmr allocators on different resources; needs ASan to detect
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

int main()
{
  using alloc_t = std::pmr::polymorphic_allocator<std::uint64_t>;

  std::array<std::byte, 4096> stack_buffer{};
  std::pmr::monotonic_buffer_resource stack_pool(stack_buffer.data(), stack_buffer.size());

  alloc_t heap_alloc(std::pmr::new_delete_resource());
  alloc_t stack_alloc(&stack_pool);

  static_assert(!std::allocator_traits<alloc_t>::propagate_on_container_move_assignment::value,
                "pmr allocators do not propagate on move");
  if (heap_alloc == stack_alloc)
  {
    std::println(std::cerr, "unexpected: pmr allocators with different resources compared equal");
    return 2;
  }

  {
    bitvector<std::uint64_t, alloc_t> a(256, heap_alloc);
    bitvector<std::uint64_t, alloc_t> b(256, stack_alloc);
    b.set(7);
    b.set(200);

    a = std::move(b);

    if (!a.get(7) || !a.get(200))
    {
      std::println(std::cerr, "data not transferred by move-assign");
      return 1;
    }
    // scope exit is the check: a free through the wrong resource is an ASan bad-free
  }

  std::println("ok: no cross-allocator free (move-assign respected allocator ownership)");
  return 0;
}
