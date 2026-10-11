/*
 *  alias_test.cpp
 *  Verifies that lam::bitvector is accessible
 */

import std;
import lam.bitvector;

// No using namespace lam::bitvec;

int main()
{
  lam::bitvector<> bv(100);
  bv.set(50);

  if (!bv[50])
    return 1;

  static_assert(lam::word_allocator_c<std::allocator<std::uint64_t>, std::uint64_t>);

  return 0;
}
