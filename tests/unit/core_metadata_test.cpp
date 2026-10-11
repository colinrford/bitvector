/*
 *  core_metadata_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector metadata
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

constexpr bool test_constexpr_metadata()
{
  bitvector<> bv(100);

  if (bv.size() != 100)
    return false;
  if (bv.length() != 100)
    return false;

  // ceil(100 / 64) = 2
  if (bv.word_count() != 2)
    return false;

  if (bv.data() == nullptr)
    return false;

  const auto& cbv = bv;
  if (cbv.data() == nullptr)
    return false;
  if (cbv.data() != bv.data())
    return false;

  return true;
}

static_assert(test_constexpr_metadata());

int main()
{
  bitvector<> empty;
  if (empty.size() != 0)
    return 1;
  if (empty.length() != 0)
    return 1;
  if (empty.word_count() != 0)
    return 1;
  if (empty.data() != nullptr)
    return 1;

  bitvector<> exact(64);
  if (exact.word_count() != 1)
    return 1;

  bitvector<> over(65);
  if (over.word_count() != 2)
    return 1;

  if (exact.data() == nullptr)
    return 1;

  const bitvector<> c_over(65);
  if (c_over.data() == nullptr)
    return 1;

  return 0;
}
