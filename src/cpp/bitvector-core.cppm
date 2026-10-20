/*
 *  bitvector-core.cppm – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  lam.bitvector is a c++ module
 */

export module lam.bitvector:core;

import std;

import :concepts;

namespace lam::bitvec
{

namespace stdr = std::ranges;
namespace stdv = std::views;

export
struct uninitialized_t
{};

export
inline constexpr uninitialized_t uninitialized{};

export
template<word_c Word = std::uint64_t,
         word_allocator_c<Word> Allocator = std::allocator<Word>>
class bitvector
{
public:
  using word_type = Word;
  using size_type = std::size_t;
  using allocator_type = Allocator;

  constexpr allocator_type get_allocator() const noexcept { return m_alloc; }

  static constexpr size_type bits_per_word = std::numeric_limits<word_type>
                                                ::digits;

  class bit_proxy
  {
    friend class bitvector;

    word_type* m_word_ptr = nullptr;
    word_type m_mask = 0;

    constexpr bit_proxy(word_type* w, word_type mask) : m_word_ptr(w),
                                                        m_mask(mask)
    {}

  public:
    constexpr bit_proxy(bit_proxy const&) = default;
    constexpr bit_proxy(bit_proxy&&) = default;
    constexpr ~bit_proxy() = default;

    constexpr bit_proxy& operator=(bool val);
    constexpr bit_proxy& operator=(const bit_proxy& other);
    constexpr operator bool() const;
    constexpr bool operator~() const;
    constexpr void flip();
  };

private:
  using allocator_traits = std::allocator_traits<Allocator>;

  static constexpr bool move_assignment_noexcept =
    allocator_traits::propagate_on_container_move_assignment::value ||
    allocator_traits::is_always_equal::value;

  static constexpr bool swap_noexcept =
    allocator_traits::propagate_on_container_swap::value ||
    allocator_traits::is_always_equal::value;

  [[no_unique_address]] Allocator m_alloc;
  word_type* m_data = nullptr;
  size_type m_num_bits = 0;
  size_type m_capacity_words = 0;

  [[nodiscard]]
  constexpr size_type words_needed(size_type num_bits) const noexcept;
  [[nodiscard]] constexpr word_type tail_mask() const noexcept;
  constexpr void allocate_storage(size_type num_bits, bool init_zeros = true);
  constexpr void deallocate_storage();

public:
  constexpr explicit bitvector(size_type num_bits, Allocator const& alloc = Allocator());
  constexpr bitvector(size_type num_bits, uninitialized_t, Allocator const& alloc = Allocator());

  template<word_source_c<word_type> Range>
  constexpr explicit bitvector(Range&& source, Allocator const& alloc = Allocator());

  constexpr bitvector();
  constexpr ~bitvector();

  constexpr bitvector(bitvector const& other);
  constexpr bitvector(bitvector&& other) noexcept;

  constexpr bitvector& operator=(bitvector const& other);
  constexpr bitvector& operator=(bitvector&& other)
    noexcept(move_assignment_noexcept);

  [[nodiscard]] constexpr bool get(size_type index) const;
  constexpr bool operator[](size_type index) const;
  constexpr bit_proxy operator[](size_type index);

  constexpr void set(size_type index, bool value = true);
  constexpr void toggle(size_type index);
  constexpr void clear(size_type index);

  [[nodiscard]] constexpr size_type length() const noexcept;
  [[nodiscard]] constexpr size_type size() const noexcept;

  [[nodiscard]] constexpr size_type word_count() const noexcept;

  [[nodiscard]] constexpr size_type count() const noexcept;
  [[nodiscard]] constexpr bool any() const noexcept;
  [[nodiscard]] constexpr bool all() const noexcept;
  [[nodiscard]] constexpr bool none() const noexcept;
  constexpr void reset() noexcept;
  constexpr void set_all() noexcept;
  constexpr void flip_all() noexcept;

  [[nodiscard]] constexpr std::span<word_type const> words() const noexcept
  { return {m_data, m_capacity_words}; }

  constexpr void print() const;

  template<word_c W, word_allocator_c<W> A>
  friend constexpr bitvector<W, A>
  operator&(bitvector<W, A> const& lhs, bitvector<W, A> const& rhs);

  template<word_c W, word_allocator_c<W> A>
  friend constexpr bitvector<W, A>
  operator|(bitvector<W, A> const& lhs, bitvector<W, A> const& rhs);

  template<word_c W, word_allocator_c<W> A>
  friend constexpr bitvector<W, A>
  operator^(bitvector<W, A> const& lhs, bitvector<W, A> const& rhs);

  template<word_c W, word_allocator_c<W> A>
  friend constexpr bitvector<W, A>
  operator~(bitvector<W, A> const& bv);

  template<word_c W, word_allocator_c<W> A>
  friend constexpr bitvector<W, A>&
  operator&=(bitvector<W, A>& lhs, bitvector<W, A> const& rhs);

  template<word_c W, word_allocator_c<W> A>
  friend constexpr bitvector<W, A>&
  operator|=(bitvector<W, A>& lhs, bitvector<W, A> const& rhs);

  template<word_c W, word_allocator_c<W> A>
  friend constexpr bitvector<W, A>&
  operator^=(bitvector<W, A>& lhs, bitvector<W, A> const& rhs);

  friend constexpr void swap(bitvector& a, bitvector& b)
    noexcept(swap_noexcept)
  {
    constexpr bool pocs = allocator_traits::propagate_on_container_swap
                                          ::value;
    constexpr bool is_always_equal = allocator_traits::is_always_equal
                                                     ::value;
    using std::swap;

    if constexpr (pocs)
      swap(a.m_alloc, b.m_alloc);
    else if constexpr (!is_always_equal)
    {
      if (a.m_alloc != b.m_alloc)
        throw std::invalid_argument(
          "bitvector::swap unequal nonpropagating allocators"
        );
    }

    swap(a.m_data, b.m_data);
    swap(a.m_num_bits, b.m_num_bits);
    swap(a.m_capacity_words, b.m_capacity_words);
  }
};

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>::bit_proxy&
bitvector<Word, Allocator>::bit_proxy::operator=(bool val)
{
  if (val)
    *m_word_ptr |= m_mask;
  else
    *m_word_ptr &= ~m_mask;
  return *this;
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>::bit_proxy&
bitvector<Word, Allocator>::bit_proxy::operator=(bit_proxy const& other)
{
  if (this == &other)
    return *this;
  return *this = static_cast<bool>(other);
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>::bit_proxy::operator bool() const
{ return (*m_word_ptr & m_mask) != 0; }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bool bitvector<Word, Allocator>::bit_proxy::operator~() const
{ return !static_cast<bool>(*this); }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::bit_proxy::flip()
{ *m_word_ptr ^= m_mask; }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr typename bitvector<Word, Allocator>::size_type
bitvector<Word, Allocator>::words_needed(size_type num_bits) const noexcept
{ return num_bits / bits_per_word + (num_bits % bits_per_word != 0); }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr typename bitvector<Word, Allocator>::word_type
bitvector<Word, Allocator>::tail_mask() const noexcept
{
  auto remaining = m_num_bits % bits_per_word;

  return remaining == 0
    ? std::numeric_limits<word_type>::max()
    : static_cast<word_type>((word_type{1} << remaining) - 1);
}

// TODO: make a decision about uninitialized storage
template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::allocate_storage(size_type num_bits,
                                                            bool init_zeros)
{
  if (num_bits == 0)
  {
    m_data = nullptr;
    m_capacity_words = 0;
    return;
  }
  m_capacity_words = words_needed(num_bits);
  m_data = allocator_traits::allocate(m_alloc, m_capacity_words);

  if (init_zeros || std::is_constant_evaluated())
    for (size_type i = 0; i < m_capacity_words; ++i)
      allocator_traits::construct(m_alloc, m_data + i, 0);
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::deallocate_storage()
{
  if (m_data)
  {
    allocator_traits::deallocate(m_alloc, m_data, m_capacity_words);
    m_data = nullptr;
    m_capacity_words = 0;
  }
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>::bitvector(size_type num_bits,
                                                Allocator const& alloc)
  : m_alloc(alloc), m_num_bits(num_bits)
{ allocate_storage(num_bits, true); }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>::bitvector(size_type num_bits,
                                                uninitialized_t,
                                                Allocator const& alloc)
  : m_alloc(alloc), m_num_bits(num_bits)
{ allocate_storage(num_bits, false); }

template<word_c Word, word_allocator_c<Word> Allocator>
template<word_source_c<Word> Range>
constexpr bitvector<Word, Allocator>::bitvector(Range&& source,
                                                Allocator const& alloc)
  : m_alloc(alloc)
{
  size_type num_words = std::ranges::size(source);

  if (num_words > std::numeric_limits<size_type>::max() / bits_per_word)
    throw std::length_error("bitvector: source too large");

  m_num_bits = num_words * bits_per_word;
  allocate_storage(m_num_bits, false);

  // problem child, can leak if iterating source throws after allocating
  stdr::copy(std::forward<Range>(source), m_data);
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>::bitvector() : bitvector(0)
{}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>::~bitvector()
{ deallocate_storage(); }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>::bitvector(bitvector const& other)
  : m_alloc(allocator_traits
          ::select_on_container_copy_construction(other.m_alloc)),
    m_num_bits(other.m_num_bits)
{
  allocate_storage(m_num_bits, false); // don't zero, copy immediately
  stdr::copy(other.words(), m_data);
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>::bitvector(bitvector&& other) noexcept
  : m_alloc(std::move(other.m_alloc)),
    m_data(other.m_data),
    m_num_bits(other.m_num_bits),
    m_capacity_words(other.m_capacity_words)
{
  other.m_data = nullptr;
  other.m_num_bits = 0;
  other.m_capacity_words = 0;
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>&
bitvector<Word, Allocator>::operator=(bitvector const& other)
{
  if (this == &other)
    return *this;

  constexpr bool pocca = allocator_traits
                       ::propagate_on_container_copy_assignment
                       ::value;

  bool incompatible = false;
  if constexpr (pocca)
    incompatible = m_alloc != other.m_alloc;

  if (incompatible || m_capacity_words != other.m_capacity_words)
  {
    // choose allocator that will own replacement
    Allocator const& target_alloc = pocca ? other.m_alloc : m_alloc;
    bitvector replacement(other.m_num_bits, uninitialized, target_alloc);

    stdr::copy(other.words(), replacement.m_data);

    if constexpr (pocca)
    {
      if (incompatible)
      {
        // release old storage through old allocator
        deallocate_storage();
        m_num_bits = 0;
      }
      m_alloc = other.m_alloc;
    }

    using std::swap;
    swap(m_data, replacement.m_data);
    swap(m_num_bits, replacement.m_num_bits);
    swap(m_capacity_words, replacement.m_capacity_words);
  } else
  {
    // reuse storage
    if constexpr (pocca)
      m_alloc = other.m_alloc;

    stdr::copy(other.words(), m_data);

    m_num_bits = other.m_num_bits;
  }

  return *this;
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>&
bitvector<Word, Allocator>::operator=(bitvector&& other)
  noexcept(move_assignment_noexcept)
{
  if (this == &other)
    return *this;

  constexpr bool pocma = allocator_traits
                       ::propagate_on_container_move_assignment
                       ::value;
  constexpr bool always_equal = allocator_traits::is_always_equal::value;

  if constexpr (!pocma && !always_equal)
  {
    if (m_alloc != other.m_alloc)
    {
      if (m_capacity_words != other.m_capacity_words)
      {
        bitvector replacement(other.m_num_bits, uninitialized, m_alloc);

        stdr::copy(other.words(), replacement.m_data);

        using std::swap;
        swap(m_data, replacement.m_data);
        swap(m_num_bits, replacement.m_num_bits);
        swap(m_capacity_words, replacement.m_capacity_words);
      } else
      {
        stdr::copy(other.words(), m_data);

        m_num_bits = other.m_num_bits;
      }
      // source remains unchanged
      return *this;
    }
  }

  deallocate_storage();

  if constexpr (pocma)
    m_alloc = std::move(other.m_alloc);

  m_data = std::exchange(other.m_data, nullptr);
  m_num_bits = std::exchange(other.m_num_bits, size_type{0});
  m_capacity_words = std::exchange(other.m_capacity_words, size_type{0});

  return *this;
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bool bitvector<Word, Allocator>::get(size_type index) const
{
  if (index >= m_num_bits)
    throw std::out_of_range("bitvector::get");
  return (m_data[index / bits_per_word] >> (index % bits_per_word)) & word_type{1};
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bool bitvector<Word, Allocator>::operator[](size_type index) const
{ return (m_data[index / bits_per_word] >> (index % bits_per_word)) & word_type{1}; }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>::bit_proxy
bitvector<Word, Allocator>::operator[](size_type index)
{ return bit_proxy(m_data + (index / bits_per_word), word_type{1} << (index % bits_per_word)); }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::set(size_type index, bool value)
{
  if (index >= m_num_bits)
    throw std::out_of_range("bitvector::set");
  if (value)
    m_data[index / bits_per_word] |= (word_type{1} << (index % bits_per_word));
  else
    m_data[index / bits_per_word] &= ~(word_type{1} << (index % bits_per_word));
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::toggle(size_type index)
{
  if (index >= m_num_bits)
    throw std::out_of_range("bitvector::toggle");
  m_data[index / bits_per_word] ^= (word_type{1} << (index % bits_per_word));
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::clear(size_type index)
{ set(index, false); }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr typename bitvector<Word, Allocator>::size_type
bitvector<Word, Allocator>::length() const noexcept
{ return m_num_bits; }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr typename bitvector<Word, Allocator>::size_type
bitvector<Word, Allocator>::size() const noexcept
{ return m_num_bits; }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr typename bitvector<Word, Allocator>::size_type
bitvector<Word, Allocator>::word_count() const noexcept
{ return m_capacity_words; }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr typename bitvector<Word, Allocator>::size_type
bitvector<Word, Allocator>::count() const noexcept
{
  return stdr::fold_left(words()
                        | stdv::transform([](word_type word) {
                          return static_cast<size_type>(std::popcount(word));
                        }),
                        size_type{0},
                        std::plus{}
                      );
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bool bitvector<Word, Allocator>::any() const noexcept
{
  return stdr::any_of(words(),
                      [](word_type word) { return word != 0; }
                    );
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bool bitvector<Word, Allocator>::all() const noexcept
{
  if (m_num_bits == 0)
    return true;

  size_type full_words = m_num_bits / bits_per_word;
  constexpr auto is_full = [](word_type word) -> bool {
                              return word == std::numeric_limits<word_type>
                                                ::max();
                            };
  if (!stdr::all_of(words().first(full_words), is_full))
    return false;

  return (m_data[m_capacity_words - 1] & tail_mask()) == tail_mask();
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bool bitvector<Word, Allocator>::none() const noexcept
{ return !any(); }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::reset() noexcept
{ stdr::fill(std::span<word_type>{m_data, m_capacity_words}, word_type{0}); }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::set_all() noexcept
{
  stdr::fill(std::span<word_type>{m_data, m_capacity_words},
             std::numeric_limits<word_type>::max()
           );

  if (m_capacity_words != 0)
    m_data[m_capacity_words - 1] &= tail_mask();
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::flip_all() noexcept
{
  stdr::transform(words(), m_data, std::bit_not<word_type>{});

  if (m_capacity_words != 0)
    m_data[m_capacity_words - 1] &= tail_mask();
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::print() const
{
  for (size_type i = 0; i < m_num_bits; ++i)
    std::print("{}", get(i) ? "1" : "0");
  std::println("");
}

} // namespace lam::bitvec

template<typename Word, typename Alloc>
struct std::formatter<lam::bitvec::bitvector<Word, Alloc>> : std::formatter<std::string> // NOLINT(cert-dcl58-cpp)
{
  auto format(lam::bitvec::bitvector<Word, Alloc> const& bv,
              std::format_context& ctx) const
  {
    std::string s;
    s.reserve(bv.size());
    for (std::size_t i = 0; i < bv.size(); ++i)
      s += bv[i] ? '1' : '0';
    return std::formatter<std::string>::format(s, ctx);
  }
};
