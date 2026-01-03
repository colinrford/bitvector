/*
 *  core_metadata_test.cpp – written by Colin Ford
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
  
  // Test size() and length()
  if (bv.size() != 100) 
    return false;
  if (bv.length() != 100) 
    return false;
  
  // Test word_count()
  // 100 bits requires ceil(100/64) = 2 words
  if (bv.word_count() != 2) 
    return false;
  
  // Test data()
  if (bv.data() == nullptr) 
    return false;
  
  // Const access check
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
  // Test 1: Empty vector
  bitvector<> empty;
  if (empty.size() != 0) 
    return 1;
  if (empty.length() != 0) 
    return 1;
  if (empty.word_count() != 0) 
    return 1;
  if (empty.data() != nullptr) 
    return 1;
  
  // Test 2: Single word boundary
  bitvector<> exact(64);
  if (exact.word_count() != 1) 
    return 1;
  
  // Test 3: Just over boundary
  bitvector<> over(65);
  if (over.word_count() != 2) 
    return 1;
  
  // Test 4: Verify data pointer is valid (runtime check)
  if (exact.data() == nullptr) 
    return 1;
  
  // Test 5: Const overload
  const bitvector<> c_over(65);
  if (c_over.data() == nullptr) 
    return 1;
  
  return 0;
}
