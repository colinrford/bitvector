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

namespace stdv = std::views;

export
template<word_c Word = std::uint64_t, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>
operator&(bitvector<Word, Allocator> const& lhs,
          bitvector<Word, Allocator> const& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  using size_type = typename bitvector<Word, Allocator>::size_type;
  bitvector<Word, Allocator> result(lhs.size(),
                                    uninitialized,
                                    lhs.get_allocator());

  const auto lhs_words = lhs.words();
  const auto rhs_words = rhs.words();

  for (auto i : stdv::iota(size_type{0}, lhs.word_count()))
    result.m_data[i] = lhs_words[i] & rhs_words[i];

  return result;
}

export
template<word_c Word = std::uint64_t, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>
operator|(bitvector<Word, Allocator> const& lhs,
          bitvector<Word, Allocator> const& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  using size_type = typename bitvector<Word, Allocator>::size_type;

  bitvector<Word, Allocator> result(lhs.size(),
                                    uninitialized,
                                    lhs.get_allocator());

  const auto lhs_words = lhs.words();
  const auto rhs_words = rhs.words();

  for (auto i : stdv::iota(size_type{0}, lhs.word_count()))
    result.m_data[i] = lhs_words[i] | rhs_words[i];

  return result;
}

export
template<word_c Word = std::uint64_t, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>
operator^(bitvector<Word, Allocator> const& lhs,
          bitvector<Word, Allocator> const& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  using size_type = typename bitvector<Word, Allocator>::size_type;

  bitvector<Word, Allocator> result(lhs.size(),
                                    uninitialized,
                                    lhs.get_allocator());

  const auto lhs_words = lhs.words();
  const auto rhs_words = rhs.words();

  for (auto i : stdv::iota(size_type{0}, lhs.word_count()))
    result.m_data[i] = lhs_words[i] ^ rhs_words[i];

  return result;
}

export
template<word_c Word = std::uint64_t, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>
operator~(bitvector<Word, Allocator> const& bv)
{
  using size_type = typename bitvector<Word, Allocator>::size_type;

  bitvector<Word, Allocator> result(bv.size(),
                                    uninitialized,
                                    bv.get_allocator());

  const auto bv_words = bv.words();

  for (auto i : stdv::iota(size_type{0}, bv.word_count()))
    result.m_data[i] = ~bv_words[i];

  size_type remaining = bv.size() % bitvector<Word, Allocator>::bits_per_word;
  if (remaining > 0)
    result.m_data[bv.word_count() - 1] &= (Word{1} << remaining) - 1;

  return result;
}

export
template<word_c Word = std::uint64_t, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>&
operator&=(bitvector<Word, Allocator>& lhs,
           bitvector<Word, Allocator> const& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  using size_type = typename bitvector<Word, Allocator>::size_type;
  const auto rhs_words = rhs.words();

  for (auto i : stdv::iota(size_type{0}, lhs.word_count()))
    lhs.m_data[i] &= rhs_words[i];

  return lhs;
}

export
template<word_c Word = std::uint64_t, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>&
operator|=(bitvector<Word, Allocator>& lhs,
           bitvector<Word, Allocator> const& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  using size_type = typename bitvector<Word, Allocator>::size_type;
  const auto rhs_words = rhs.words();

  for (auto i : stdv::iota(size_type{0}, lhs.word_count()))
    lhs.m_data[i] |= rhs_words[i];

  return lhs;
}

export
template<word_c Word = std::uint64_t, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>&
operator^=(bitvector<Word, Allocator>& lhs,
           bitvector<Word, Allocator> const& rhs)
{
  if (lhs.size() != rhs.size())
    throw std::invalid_argument("bitvector operands must have same size");

  using size_type = typename bitvector<Word, Allocator>::size_type;
  const auto rhs_words = rhs.words();

  for (auto i : stdv::iota(size_type{0}, lhs.word_count()))
    lhs.m_data[i] ^= rhs_words[i];

  return lhs;
}

} // namespace lam::bitvec
