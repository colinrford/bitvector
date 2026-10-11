/*
 *  core_proxy_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector bit_proxy
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

constexpr bool test_constexpr_proxy()
{
  bitvector<> bv(10);

  static_assert(std::same_as<decltype(bv[0]), bitvector<>::bit_proxy>);
  static_assert(std::same_as<decltype(bv.operator[](0)), bitvector<>::bit_proxy>);

  const auto& cbv = bv;
  static_assert(std::same_as<decltype(cbv[0]), bool>);

  bv[0] = true;
  if (!bv[0])
    return false;

  bv[0] = false;
  if (bv[0])
    return false;

  bv[1] = true;
  bv[2] = false;
  bv[2] = bv[1];
  if (!bv[2])
    return false;

  bv[3] = false;
  bv[3].flip();
  if (!bv[3])
    return false;

  bv[3].flip();
  if (bv[3])
    return false;

  bv[4] = true;
  if (~bv[4] != false)
    return false;

  bv[4] = false;
  if (~bv[4] != true)
    return false;

  return true;
}

static_assert(test_constexpr_proxy());

int main()
{
  bitvector<> bv(100);
  static_assert(std::same_as<decltype(bv[0]), bitvector<>::bit_proxy>);
  bv[50] = true;
  if (bv[50] != true)
    return 1;

  bv[10] = bv[20] = true;
  if (!bv[10] || !bv[20])
    return 1;

  bv[99] = false;
  bv[99].flip();
  if (!bv.get(99))
    return 1;

  // Test 4: Ensure proxy holds reference to original data
  auto p = bv[0];
  bv[0] = true;
  if (!p)
    return 1;

  p = false;
  if (bv[0])
    return 1;

  return 0;
}
