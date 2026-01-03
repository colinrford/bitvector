/*
 *  core_print_test.cpp – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector printing
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

int main()
{
  bitvector<> fmt_bv(4);  // 0000
  fmt_bv.set(1);          // 0010
  fmt_bv.set(3);          // 1010  (bit 3 and 1 set)
  
  // Format string check
  // Indices: 0->0, 1->1, 2->0, 3->1. result "0101"
  std::string s = std::format("{}", fmt_bv);
  if (s != "0101") 
    return 1;
  
  fmt_bv.print();
  std::print("Explicit std::print test: {}\n", fmt_bv);

  return 0;
}
