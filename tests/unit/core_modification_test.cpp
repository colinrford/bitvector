/*
 *  core_modification_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector core modification
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

constexpr bool test_constexpr_modification()
{
  bitvector<> bv(10);

  bv.toggle(5);
  if (!bv.get(5))
    return false;
  bv.toggle(5);
  if (bv.get(5))
    return false;

  bv.set(2);
  bv.clear(2);
  if (bv.get(2))
    return false;

  return true;
}

static_assert(test_constexpr_modification());

int main()
{
  bitvector<> bv(64);

  try
  {
    bv.set(64);
    return 1;
  }
  catch (const std::out_of_range&)
  {}

  try
  {
    bv.toggle(64);
    return 1;
  }
  catch (const std::out_of_range&)
  {}

  bv.toggle(32);
  if (!bv.get(32))
    return 1;
  bv.toggle(32);
  if (bv.get(32))
    return 1;

  bv.set(32);
  bv.clear(32);
  if (bv.get(32))
    return 1;

  return 0;
}
