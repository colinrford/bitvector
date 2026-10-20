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

namespace stdr = std::ranges;
namespace stdv = std::views;

export
template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>
operator&(bitvector<Word, Allocator> const& lhs,
          bitvector<Word, Allocator> const& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  bitvector<Word, Allocator> result(lhs.size(),
                                    uninitialized,
                                    lhs.get_allocator());

  stdr::transform(lhs.words(),
                  rhs.words(),
                  result.m_data,
                  [](auto a, auto b) -> Word { return a & b; }
                );

  return result;
}

export
template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>
operator|(bitvector<Word, Allocator> const& lhs,
          bitvector<Word, Allocator> const& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  bitvector<Word, Allocator> result(lhs.size(),
                                    uninitialized,
                                    lhs.get_allocator());

  stdr::transform(lhs.words(),
                  rhs.words(),
                  result.m_data,
                  [](auto a, auto b) -> Word { return a | b; }
                );

  return result;
}

export
template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>
operator^(bitvector<Word, Allocator> const& lhs,
          bitvector<Word, Allocator> const& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  bitvector<Word, Allocator> result(lhs.size(),
                                    uninitialized,
                                    lhs.get_allocator());

  stdr::transform(lhs.words(),
                  rhs.words(),
                  result.m_data,
                  [](auto a, auto b) -> Word { return a ^ b; }
                );

  return result;
}

export
template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>
operator~(bitvector<Word, Allocator> const& bv)
{
  bitvector<Word, Allocator> result(bv.size(),
                                    uninitialized,
                                    bv.get_allocator());

  stdr::transform(bv.words(),
                  result.m_data,
                  [](auto word) -> Word { return ~word; }
                );

  if (result.m_capacity_words != 0)
    result.m_data[result.m_capacity_words - 1] &= result.tail_mask();

  return result;
}

export
template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>&
operator&=(bitvector<Word, Allocator>& lhs,
           bitvector<Word, Allocator> const& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  stdr::transform(lhs.words(),
                  rhs.words(),
                  lhs.m_data,
                  [](auto a, auto b) -> Word { return a & b; }
                );

  return lhs;
}

export
template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>&
operator|=(bitvector<Word, Allocator>& lhs,
           bitvector<Word, Allocator> const& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  stdr::transform(lhs.words(),
                  rhs.words(),
                  lhs.m_data,
                  [](auto a, auto b) -> Word { return a | b; }
                );

  return lhs;
}

export
template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>&
operator^=(bitvector<Word, Allocator>& lhs,
           bitvector<Word, Allocator> const& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  stdr::transform(lhs.words(),
                  rhs.words(),
                  lhs.m_data,
                  [](auto a, auto b) -> Word { return a ^ b; }
                );

  return lhs;
}

} // namespace lam::bitvec
