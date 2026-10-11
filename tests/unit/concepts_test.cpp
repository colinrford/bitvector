/*
 *  concepts_test.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  unit test for lam.bitvector module concepts
 */

import std;
import lam.bitvector;

using namespace lam::bitvec;

/*
 *  word_c checks
 */
static_assert(word_c<std::uint8_t>);
static_assert(word_c<std::uint16_t>);
static_assert(word_c<std::uint32_t>);
static_assert(word_c<std::uint64_t>);

static_assert(!word_c<bool>);
static_assert(!word_c<char8_t>);
static_assert(!word_c<float>);
static_assert(!word_c<double>);

/*
 *  word_allocator_c checks
 */
static_assert(word_allocator_c<std::allocator<std::uint64_t>, std::uint64_t>);
static_assert(!word_allocator_c<std::allocator<int>, std::uint64_t>);

/*
 *  word_sink_c checks
 */
static_assert(word_sink_c<std::span<std::uint64_t>, std::uint64_t>);
static_assert(word_sink_c<std::array<std::uint64_t, 4>, std::uint64_t>);
static_assert(word_sink_c<std::vector<std::uint64_t>, std::uint64_t>);

/*
 *  word_source_c checks
 */
static_assert(word_source_c<std::vector<std::uint64_t>, std::uint64_t>);

int main()
{}
