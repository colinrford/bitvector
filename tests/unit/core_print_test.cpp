/*
 *  core_print_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector printing
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

int main()
{
  bitvector<> fmt_bv(4);
  fmt_bv.set(1);
  fmt_bv.set(3);

  // index 0 prints first
  std::string s = std::format("{}", fmt_bv);
  if (s != "0101")
    return 1;

  fmt_bv.print();
  std::print("Explicit std::print test: {}\n", fmt_bv);

  return 0;
}
