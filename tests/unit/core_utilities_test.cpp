/*
 *  core_utilities_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector core utilities
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

constexpr bool test_constexpr_bulk()
{
  bitvector<> bv(128);

  if (!bv.none())
    return false;
  if (bv.any())
    return false;
  if (bv.all())
    return false;

  bv.set_all();
  if (!bv.all())
    return false;
  if (!bv.any())
    return false;
  if (bv.none())
    return false;
  if (bv.count() != 128)
    return false;

  bv.reset();
  if (bv.count() != 0)
    return false;

  bv.flip_all();
  if (bv.count() != 128)
    return false;

  return true;
}

static_assert(test_constexpr_bulk());

int main()
{
  bitvector<> bv(64);
  bv.set(0);
  bv.set(63);
  if (bv.count() != 2)
    return 1;

  // Test all() with partial word
  bitvector<> small(3);
  small.set_all();
  if (!small.all())
    return 1;
  if (small.count() != 3)
    return 1;

  small.set(1, false);
  if (small.all())
    return 1;

  small.reset();
  if (small.any())
    return 1;
  small.set(2);
  if (!small.any())
    return 1;

  bitvector<> flip_test(10);
  flip_test.set(0);
  flip_test.set(2);
  flip_test.flip_all();

  if (flip_test.get(0))
    return 1;
  if (!flip_test.get(1))
    return 1;
  if (flip_test.get(2))
    return 1;
  if (!flip_test.get(9))
    return 1;
  if (flip_test.count() != 8)
    return 1;

  return 0;
}
