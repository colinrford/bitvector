/*
 *  bitvector-concepts.cppm – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  lam.bitvector is a c++ module
 */

export module lam.bitvector:concepts;

import std;

namespace lam::bitvec
{

export
template<typename Word>
concept word_c = std::unsigned_integral<Word>
             and requires(Word w) { std::popcount(w); };

template<typename Allocator, typename Word>
concept raw_word_allocation_c = requires(Allocator a, Word* p, std::size_t n)
{
  { a.allocate(n) } -> std::same_as<Word*>;
  a.deallocate(p, n);
};

export
template<typename Allocator, typename Word>
concept word_allocator_c = word_c<Word>
                       and std::same_as<typename Allocator::value_type, Word>
                       and std::copy_constructible<Allocator>
                       and std::equality_comparable<Allocator>
                       and raw_word_allocation_c<Allocator, Word>;

export
template<typename Range, typename Word>
concept word_sink_c = word_c<Word>
                  and std::ranges::output_range<Range, Word>
                  and std::ranges::sized_range<Range>
                  and std::same_as<std::ranges::range_value_t<Range>, Word>;

export
template<typename Range, typename Word>
concept word_source_c = word_c<Word>
                  and std::ranges::input_range<Range>
                  and std::ranges::sized_range<Range>
                  and std::same_as<std::ranges::range_value_t<Range>, Word>;

} // end namespace lam::bitvec
