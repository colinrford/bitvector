/*
 *  core_utilities_test.cpp – written by Colin Ford
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

  // All/None/Any empty/zeros
  if (!bv.none())
    return false;
  if (bv.any())
    return false;
  // all() is true if all bits are 1. Here they are 0.
  // Wait, check implementation: if 128 bits are 0, all() should be false.
  // Unless size is 0? Here size is 128.
  if (bv.all())
    return false;

  // Set All
  bv.set_all();
  if (!bv.all())
    return false;
  if (!bv.any())
    return false;
  if (bv.none())
    return false;
  if (bv.count() != 128)
    return false;

  // Reset
  bv.reset();
  if (bv.count() != 0)
    return false;

  // Flip All
  bv.flip_all();
  if (bv.count() != 128)
    return false;

  return true;
}

static_assert(test_constexpr_bulk());

int main()
{
  // Test count patterns
  bitvector<> bv(64);
  bv.set(0);
  bv.set(63);
  if (bv.count() != 2)
    return 1;

  // Test all() with partial word
  bitvector<> small(3); // 3 bits
  small.set_all();
  // Internal word might have garbage if logic is wrong, but set_all should handle it
  // Implementation check: set_all sets words to ~0.
  // all() checks masked bits.
  if (!small.all())
    return 1;
  if (small.count() != 3)
    return 1;

  small.set(1, false);
  if (small.all())
    return 1;

  // Test any()
  small.reset();
  if (small.any())
    return 1;
  small.set(2);
  if (!small.any())
    return 1;

  // Test flip_all() at runtime
  bitvector<> flip_test(10);
  flip_test.set(0);
  flip_test.set(2);
  flip_test.flip_all();

  // Originally 10100... -> 0000000000...
  // Flipped:   0101111111...

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
