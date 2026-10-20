/*
 *  core_construction_test.cpp – Colin Ford
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

  bitvector<> uninit_bv(30, uninitialized);
  if (uninit_bv.size() != 30)
    return false;

  bitvector<> src_move(10);
  src_move.set(1);
  bitvector<> dst_move(std::move(src_move));

  if (dst_move.size() != 10)
    return false;
  if (!dst_move.get(1))
    return false;

  if (src_move.size() != 0 || !src_move.words().empty())
    return false;

  return true;
}

static_assert(test_constexpr_construction());

int main()
{
  constexpr bitvector<> default_bv{};
  static_assert(default_bv.size() == 0);
  static_assert(default_bv.word_count() == 0);

  // volatile keeps n a run-time value
  volatile std::size_t n = 50;
  bitvector<> runtime_bv(n);

  if (runtime_bv.size() != 50 || runtime_bv.word_count() != 1 // (50 + 63) / 64 = 1
      || runtime_bv.count() != 0)
    return 1;

  bitvector<> copy_bv = runtime_bv;
  if (copy_bv.size() != 50 || copy_bv.count() != 0)
    return 1;

  bitvector<> src_move(100);
  src_move.set(10);

  bitvector<> dst_move(std::move(src_move));

  if (dst_move.size() != 100)
    return 1;
  if (!dst_move.get(10))
    return 1;

  if (src_move.size() != 0)
    return 1;
  if (!src_move.words().empty())
    return 1;

  return 0;
}
