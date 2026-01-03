/*
 *  core_swap_test.cpp – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector core swap
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

int main()
{
  bitvector<> a(100);
  a.set_all();
  
  bitvector<> b(50);
  b.reset();
  
  using std::swap;
  swap(a, b);
  
  if (a.size() != 50)
    return 1;
  if (a.count() != 0)
    return 1;
  
  if (b.size() != 100)
    return 1;
  if (b.count() != 100)
    return 1;
  
  return 0;
}
