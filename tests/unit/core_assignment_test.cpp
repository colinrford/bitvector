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

constexpr bool test_copy_assignment_boundaries()
{
  bitvector<> source(100);
  source.set(1);
  source.set(99);
  bitvector<> dest(10);
  dest.set_all();

  if (&(dest = source) != &dest || dest.size() != 100 || dest.count() != 2
      || !dest.get(1) || !dest.get(99))
    return false;

  auto& self = dest;
  dest = self;
  if (dest.size() != 100 || dest.count() != 2 || !dest.get(99))
    return false;

  dest.clear(99);
  if (!source.get(99))
    return false;

  bitvector<> empty;
  dest = empty;
  if (dest.size() != 0 || !dest.words().empty() || dest.count() != 0)
    return false;
  empty = dest;
  return empty.size() == 0 && empty.words().empty();
}

static_assert(test_copy_assignment_boundaries());

constexpr bool test_move_assignment_boundaries()
{
  bitvector<> source(100);
  source.set(99);
  bitvector<> dest(10);
  dest.set_all();
  if (&(dest = std::move(source)) != &dest
      || dest.size() != 100 || dest.count() != 1 || !dest.get(99)
      || source.size() != 0 || !source.words().empty())
    return false;

  auto& self = dest;
  dest = std::move(self);
  if (dest.size() != 100 || dest.count() != 1 || !dest.get(99))
    return false;

  // A moved-from object remains assignable.
  source = std::move(dest);
  if (source.size() != 100 || !source.get(99)
      || dest.size() != 0 || !dest.words().empty())
    return false;

  bitvector<> empty;
  source = std::move(empty);
  if (source.size() != 0 || !source.words().empty()
      || empty.size() != 0 || !empty.words().empty())
    return false;
  dest = std::move(empty);
  return dest.size() == 0 && dest.words().empty();
}

static_assert(test_move_assignment_boundaries());

int main()
{
  if (!test_move_assignment_boundaries())
    return 1;

  if (!test_copy_assignment_boundaries())
    return 1;

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
