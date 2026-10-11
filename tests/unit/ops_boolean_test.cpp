/*
 *  ops_boolean_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for bitvector boolean ops
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

constexpr bool test_constexpr_ops()
{
  bitvector<> a(4);
  bitvector<> b(4);

  a.set(0);
  a.set(2); // 1010, index 0 first
  b.set(1);
  b.set(2); // 0110

  // 0010
  bitvector<> c = a & b;
  if (c.count() != 1 || !c.get(2))
    return false;

  // 1110
  bitvector<> d = a | b;
  if (d.count() != 3 || !d.get(0) || !d.get(1) || !d.get(2))
    return false;

  // 1100
  bitvector<> e = a ^ b;
  if (e.count() != 2 || !e.get(0) || !e.get(1) || e.get(2))
    return false;

  // 0101
  bitvector<> f = ~a;
  if (f.count() != 2 || f.get(0) || !f.get(1) || f.get(2) || !f.get(3))
    return false;

  bitvector<> g = a;
  g &= b;
  if (g.count() != 1 || !g.get(2))
    return false;

  bitvector<> h = a; // 1010
  h |= b;            // 0110 -> 1110
  if (h.count() != 3 || !h.get(0) || !h.get(1) || !h.get(2))
    return false;

  bitvector<> i = a; // 1010
  i ^= b;            // 0110 -> 1100
  if (i.count() != 2 || !i.get(0) || !i.get(1) || i.get(2))
    return false;

  return true;
}

static_assert(test_constexpr_ops());

int main()
{
  bitvector<> a(100);
  a.set_all();

  bitvector<> b(100);
  b.reset();

  bitvector<> small(50);
  bool caught = false;
  try
  {
    [[maybe_unused]] auto res = a & small;
  }
  catch (const std::invalid_argument&)
  {
    caught = true;
  }
  if (!caught)
    return 1;
  if ((a & b).any())
    return 1;
  if (!(a | b).all())
    return 1;
  if (!(a ^ b).all())
    return 1;
  if (!(~b).all())
    return 1;
  if ((~a).any())
    return 1;

  bitvector<> g = a;
  g &= b;
  if (g.any())
    return 1;

  bitvector<> h = a;
  h |= b;
  if (!h.all())
    return 1;

  bitvector<> i = a;
  i ^= b;
  if (!i.all())
    return 1;

  return 0;
}
