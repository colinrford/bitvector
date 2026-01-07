/*
 *  core_construction_test.cpp – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector core construction
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

constexpr bool test_constexpr_construction()
{
  bitvector<> sized_bv(100);

  if (sized_bv.size() != 100 || sized_bv.word_count() != 2 || sized_bv.count() != 0)
    return false;

  // Uninitialized construction check lam::bitvec::uninitialized
  bitvector<> uninit_bv(30, uninitialized);
  if (uninit_bv.size() != 30)
    return false;

  // Move Construction (compile-time)
  bitvector<> src_move(10);
  src_move.set(1);
  bitvector<> dst_move(std::move(src_move));

  if (dst_move.size() != 10)
    return false;
  if (!dst_move.get(1))
    return false;

  // Verify moved-from state at compile-time
  if (src_move.size() != 0)
    return false;

  return true;
}

static_assert(test_constexpr_construction());

int main()
{
  // Default construction
  constexpr bitvector<> default_bv{};
  static_assert(default_bv.size() == 0);
  static_assert(default_bv.word_count() == 0);

  // Runtime construction
  // Volatile prevents compiler from optimizing it to constant if it tries properly
  volatile std::size_t n = 50;
  bitvector<> runtime_bv(n);

  if (runtime_bv.size() != 50 || runtime_bv.word_count() != 1 // (50 + 63) / 64 = 1
      || runtime_bv.count() != 0)
    return 1;

  // Also verify copy behavior at runtime
  bitvector<> copy_bv = runtime_bv;
  if (copy_bv.size() != 50 || copy_bv.count() != 0)
    return 1;

  // Verify Move Constructor
  // Create a vector, set a bit
  bitvector<> src_move(100);
  src_move.set(10);

  // Move construct
  bitvector<> dst_move(std::move(src_move)); // Explicit move

  if (dst_move.size() != 100)
    return 1;
  if (!dst_move.get(10))
    return 1;

  // Verify moved-from object is empty
  if (src_move.size() != 0)
    return 1;
  if (src_move.data() != nullptr)
    return 1;

  return 0;
}
