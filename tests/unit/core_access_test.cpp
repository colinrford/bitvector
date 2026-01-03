/*
 *  core_access_test.cpp – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector core access
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

constexpr bool test_constexpr_access()
{
  bitvector<> bv(64);
  bv.set(0);
  bv.set(63);
  
  if (!bv.get(0) || !bv[0])
    return false;
  if (bv.get(1) || bv[1])
    return false;
  if (!bv.get(63) || !bv[63])
    return false;
  
  // Test proxy assignment
  bv[1] = true;
  if (!bv.get(1))
    return false;
  
  bv[0] = false;
  if (bv.get(0))
    return false;
  
  return true;
}

static_assert(test_constexpr_access());

int main()
{
  bitvector<> bv(128);
  
  // Test 1: get() bounds check
  try {
    bv.get(128); // out of bounds
    return 1;
  } catch (const std::out_of_range&) {
    // expected
  }
  
  // Test 2: set() and get() consistency
  bv.set(10);
  bv.set(100);
  
  if (!bv.get(10))
    return 1;
  if (!bv.get(100))
    return 1;
  if (bv.get(0))
    return 1;
  
  // Test 3: Proxy access
  if (bv[10] != true)
    return 1;
  
  if (bv[0] != false)
    return 1;
  // Test 4: Proxy modification
  bv[50] = true;
  if (!bv.get(50))
    return 1;
  
  bv[10] = false;
  if (bv.get(10))
    return 1;
  
  // Test 5: Proxy to proxy assignment
  bv[10] = true;
  bv[50] = false;
  bv[50] = bv[10]; // should satisfy both being true
  if (!bv.get(50))
    return 1;
  
  // Test 6: operator~ on proxy
  bv[0] = false;
  if (~bv[0] != true)
    return 1;
  
  bv[0] = true;
  if (~bv[0] != false)
    return 1;

  return 0;
}
