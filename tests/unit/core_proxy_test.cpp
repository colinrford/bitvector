/*
 *  core_proxy_test.cpp – written by Colin Ford
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

  // Verify return type is bit_proxy
  static_assert(std::same_as<decltype(bv[0]), bitvector<>::bit_proxy>);
  static_assert(std::same_as<decltype(bv.operator[](0)), bitvector<>::bit_proxy>);

  // Verify const return type is bool
  const auto& cbv = bv;
  static_assert(std::same_as<decltype(cbv[0]), bool>);

  // Test 1: Assignment from bool
  bv[0] = true;
  if (!bv[0])
    return false;

  bv[0] = false;
  if (bv[0])
    return false;

  // Test 2: Assignment from proxy
  bv[1] = true;
  bv[2] = false;
  bv[2] = bv[1];
  if (!bv[2])
    return false;

  // Test 3: flip()
  bv[3] = false;
  bv[3].flip();
  if (!bv[3])
    return false;

  bv[3].flip();
  if (bv[3])
    return false;

  // Test 4: operator~
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
  // Test 1: Standard usage
  bv[50] = true;
  if (bv[50] != true)
    return 1;

  // Test 2: Chained assignment (if supported by return type reference)
  bv[10] = bv[20] = true;
  if (!bv[10] || !bv[20])
    return 1;

  // Test 3: Flip
  bv[99] = false;
  bv[99].flip();
  if (!bv.get(99))
    return 1;

  // Test 4: Ensure proxy holds reference to original data
  auto p = bv[0];
  bv[0] = true;
  if (!p)
    return 1; // p should reflect change in bv[0]

  p = false;
  if (bv[0])
    return 1; // bv[0] should reflect change in p

  return 0;
}
