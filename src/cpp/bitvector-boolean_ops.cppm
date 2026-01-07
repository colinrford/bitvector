/*
 *  bitvector-boolean_ops.cppm – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  lam.bitvector is a c++ module
 */
export module lam.bitvector:boolean_ops;

import std;
import :core;

namespace lam::bitvec
{

export template<bitvector_allocator_c Allocator>
constexpr bitvector<Allocator> operator&(const bitvector<Allocator>& lhs, const bitvector<Allocator>& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  using size_type = typename bitvector<Allocator>::size_type;
  // Create result with allocator from lhs (standard practice)
  bitvector<Allocator> result(lhs.size(), uninitialized, lhs.get_allocator());

  const auto* lhs_data = lhs.data();
  const auto* rhs_data = rhs.data();
  auto* res_data = result.data();
  size_type count = lhs.word_count();

  for (size_type i = 0; i < count; ++i)
    res_data[i] = lhs_data[i] & rhs_data[i];

  return result;
}

export template<bitvector_allocator_c Allocator>
constexpr bitvector<Allocator> operator|(const bitvector<Allocator>& lhs, const bitvector<Allocator>& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  using size_type = typename bitvector<Allocator>::size_type;

  bitvector<Allocator> result(lhs.size(), uninitialized, lhs.get_allocator());

  const auto* lhs_data = lhs.data();
  const auto* rhs_data = rhs.data();
  auto* res_data = result.data();
  size_type count = lhs.word_count();

  for (size_type i = 0; i < count; ++i)
    res_data[i] = lhs_data[i] | rhs_data[i];

  return result;
}

export template<bitvector_allocator_c Allocator>
constexpr bitvector<Allocator> operator^(const bitvector<Allocator>& lhs, const bitvector<Allocator>& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  using size_type = typename bitvector<Allocator>::size_type;

  bitvector<Allocator> result(lhs.size(), uninitialized, lhs.get_allocator());

  const auto* lhs_data = lhs.data();
  const auto* rhs_data = rhs.data();
  auto* res_data = result.data();
  size_type count = lhs.word_count();

  for (size_type i = 0; i < count; ++i)
    res_data[i] = lhs_data[i] ^ rhs_data[i];

  return result;
}

// Unary NOT operator
export template<bitvector_allocator_c Allocator>
constexpr bitvector<Allocator> operator~(const bitvector<Allocator>& bv)
{
  using size_type = typename bitvector<Allocator>::size_type;

  bitvector<Allocator> result(bv.size(), uninitialized, bv.get_allocator());

  const auto* bv_data = bv.data();
  auto* res_data = result.data();
  size_type count = bv.word_count();

  for (size_type i = 0; i < count; ++i)
    res_data[i] = ~bv_data[i];

  size_type remaining = bv.size() % bitvector<Allocator>::bits_per_word;
  if (remaining > 0)
    res_data[count - 1] &= (typename bitvector<Allocator>::word_type{1} << remaining) - 1;

  return result;
}

// Compound assignment operators
export template<bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>& operator&=(bitvector<Allocator>& lhs, const bitvector<Allocator>& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  using size_type = typename bitvector<Allocator>::size_type;

  auto* lhs_data = lhs.data();
  const auto* rhs_data = rhs.data();
  size_type count = lhs.word_count();

  for (size_type i = 0; i < count; ++i)
    lhs_data[i] &= rhs_data[i];

  return lhs;
}

export template<bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>& operator|=(bitvector<Allocator>& lhs, const bitvector<Allocator>& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  using size_type = typename bitvector<Allocator>::size_type;

  auto* lhs_data = lhs.data();
  const auto* rhs_data = rhs.data();
  size_type count = lhs.word_count();

  for (size_type i = 0; i < count; ++i)
    lhs_data[i] |= rhs_data[i];

  return lhs;
}

export template<bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>& operator^=(bitvector<Allocator>& lhs, const bitvector<Allocator>& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  using size_type = typename bitvector<Allocator>::size_type;

  auto* lhs_data = lhs.data();
  const auto* rhs_data = rhs.data();
  size_type count = lhs.word_count();

  for (size_type i = 0; i < count; ++i)
    lhs_data[i] ^= rhs_data[i];

  return lhs;
}

} // namespace lam::bitvec
