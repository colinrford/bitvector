/*
 *  core_assignment_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector assignment
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

constexpr bool test_constexpr_assignment()
{
  bitvector<> a(10);
  a.set(1);

  bitvector<> b(5);
  b = a;

  if (b.size() != 10)
    return false;
  if (!b.get(1))
    return false;
  // Ensure deep copy (modify b, a unchanged)
  b.set(1, false);
  if (!a.get(1))
    return false;

  bitvector<> c(20);
  c.set(5);
  bitvector<> d;
  d = std::move(c);

  if (d.size() != 20)
    return false;
  if (!d.get(5))
    return false;

  if (c.size() != 0)
    return false;
  if (!c.words().empty())
    return false;

  auto& b_self = b;
  b = b_self;
  if (b.size() != 10)
    return false;

  auto& d_self = d;
  d = std::move(d_self);
  if (d.size() != 20)
    return false;

  return true;
}

static_assert(test_constexpr_assignment());

int main()
{
  bitvector<> a(100);
  a.set_all();

  bitvector<> b;
  b = a;

  if (b.size() != 100)
    return 1;
  if (b.count() != 100)
    return 1;

  bitvector<> c;
  c = std::move(b);

  if (c.size() != 100)
    return 1;
  if (c.count() != 100)
    return 1;

  if (b.size() != 0 || !b.words().empty())
    return 1;

  return 0;
}
