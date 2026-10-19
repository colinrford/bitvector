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
    constexpr bit_proxy(const bit_proxy&) = default;
    constexpr bit_proxy(bit_proxy&&) = default;
    constexpr ~bit_proxy() = default;

    constexpr bit_proxy& operator=(bool val);
    constexpr bit_proxy& operator=(const bit_proxy& other);
    constexpr operator bool() const;
    constexpr bool operator~() const;
    constexpr void flip();
  };

private:
  [[no_unique_address]] Allocator m_alloc;
  word_type* m_data = nullptr;
  size_type m_num_bits = 0;
  size_type m_capacity_words = 0;

  [[nodiscard]] constexpr size_type words_needed(size_type num_bits) const;
  constexpr void allocate_storage(size_type num_bits, bool init_zeros = true);
  constexpr void deallocate_storage();

public:
  constexpr explicit bitvector(size_type num_bits, const Allocator& alloc = Allocator());
  constexpr bitvector(size_type num_bits, uninitialized_t, Allocator const& alloc = Allocator());

  template<word_source_c<word_type> Range>
  constexpr explicit bitvector(Range&& source, Allocator const& alloc = Allocator());

  constexpr bitvector();
  constexpr ~bitvector();

  constexpr bitvector(bitvector const& other);
  constexpr bitvector(bitvector&& other) noexcept;

  constexpr bitvector& operator=(bitvector const& other);
  constexpr bitvector& operator=(bitvector&& other) noexcept;

  [[nodiscard]] constexpr bool get(size_type index) const;
  constexpr bool operator[](size_type index) const;
  constexpr bit_proxy operator[](size_type index);

  constexpr void set(size_type index, bool value = true);
  constexpr void toggle(size_type index);
  constexpr void clear(size_type index);

  [[nodiscard]] constexpr size_type length() const;
  [[nodiscard]] constexpr size_type size() const;

  [[nodiscard]] constexpr size_type word_count() const noexcept;

  [[nodiscard]] constexpr size_type count() const noexcept;
  [[nodiscard]] constexpr bool any() const noexcept;
  [[nodiscard]] constexpr bool all() const noexcept;
  [[nodiscard]] constexpr bool none() const noexcept;
  constexpr void reset() noexcept;
  constexpr void set_all() noexcept;
  constexpr void flip_all() noexcept;

  [[nodiscard]] constexpr std::span<word_type const> words() const
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

  friend void swap(bitvector& a, bitvector& b) noexcept
  {
    using std::swap;
    swap(a.m_alloc, b.m_alloc);
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
bitvector<Word, Allocator>::words_needed(size_type num_bits) const
{ return num_bits / bits_per_word + (num_bits % bits_per_word != 0); }

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
  m_data = std::allocator_traits<Allocator>
              ::allocate(m_alloc, m_capacity_words);

  if (init_zeros || std::is_constant_evaluated())
    for (size_type i = 0; i < m_capacity_words; ++i)
      std::allocator_traits<Allocator>::construct(m_alloc, m_data + i, 0);
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::deallocate_storage()
{
  if (m_data)
  {
    if constexpr (!std::is_trivially_destructible_v<word_type>)
      for (size_type i = 0; i < m_capacity_words; ++i)
        std::allocator_traits<Allocator>::destroy(m_alloc, m_data + i);
    std::allocator_traits<Allocator>::deallocate(m_alloc,
                                                 m_data,
                                                 m_capacity_words);
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

  if (m_data)
    std::ranges::copy(std::forward<Range>(source), m_data);
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>::bitvector() : bitvector(0)
{}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>::~bitvector()
{ deallocate_storage(); }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>::bitvector(bitvector const& other)
  : m_alloc(std::allocator_traits<Allocator>
               ::select_on_container_copy_construction(other.m_alloc)),
    m_num_bits(other.m_num_bits)
{
  allocate_storage(m_num_bits, false); // don't zero, copy immediately
  if (m_data)
    std::copy(other.m_data, other.m_data + m_capacity_words, m_data);
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

  if constexpr (std::allocator_traits<Allocator>
                   ::propagate_on_container_copy_assignment
                   ::value)
  {
    if (m_alloc != other.m_alloc)
    {
      deallocate_storage();
      m_alloc = other.m_alloc;
      m_num_bits = other.m_num_bits;
      allocate_storage(m_num_bits, false);
      if (m_data)
        std::copy(other.m_data, other.m_data + m_capacity_words, m_data);
      return *this;
    }
  }

  if (m_capacity_words != other.m_capacity_words)
  {
    deallocate_storage();
    m_num_bits = other.m_num_bits;
    allocate_storage(m_num_bits, false);
  }
  else
    m_num_bits = other.m_num_bits;

  if (m_data)
    std::copy(other.m_data, other.m_data + m_capacity_words, m_data);
  return *this;
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bitvector<Word, Allocator>&
bitvector<Word, Allocator>::operator=(bitvector&& other) noexcept
{
  if (this == &other)
    return *this;

  constexpr bool pocma = std::allocator_traits<Allocator>
                            ::propagate_on_container_move_assignment
                            ::value;

  if (pocma || m_alloc == other.m_alloc)
  {
    deallocate_storage();
    if constexpr (pocma)
      m_alloc = std::move(other.m_alloc);
    m_data = other.m_data;
    m_num_bits = other.m_num_bits;
    m_capacity_words = other.m_capacity_words;

    other.m_data = nullptr;
    other.m_num_bits = 0;
    other.m_capacity_words = 0;
  }
  else
  {
    // may allocate; under the unconditional noexcept a failure terminates
    if (m_capacity_words != other.m_capacity_words)
    {
      deallocate_storage();
      m_num_bits = other.m_num_bits;
      allocate_storage(m_num_bits, false);
    }
    else
      m_num_bits = other.m_num_bits;

    if (m_data)
      std::copy(other.m_data, other.m_data + m_capacity_words, m_data);
  }
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
bitvector<Word, Allocator>::length() const
{ return m_num_bits; }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr typename bitvector<Word, Allocator>::size_type
bitvector<Word, Allocator>::size() const
{ return m_num_bits; }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr typename bitvector<Word, Allocator>::size_type
bitvector<Word, Allocator>::word_count() const noexcept
{ return m_capacity_words; }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr typename bitvector<Word, Allocator>::size_type
bitvector<Word, Allocator>::count() const noexcept
{
  size_type total = 0;
  for (size_type i = 0; i < m_capacity_words; ++i)
    total += static_cast<size_type>(std::popcount(m_data[i]));
  return total;
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bool bitvector<Word, Allocator>::any() const noexcept
{
  for (size_type i = 0; i < m_capacity_words; ++i)
    if (m_data[i] != 0)
      return true;
  return false;
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bool bitvector<Word, Allocator>::all() const noexcept
{
  if (m_num_bits == 0)
    return true;

  size_type full_words = m_num_bits / bits_per_word;
  for (size_type i = 0; i < full_words; ++i)
    if (m_data[i] != std::numeric_limits<word_type>::max())
      return false;

  size_type remaining_bits = m_num_bits % bits_per_word;
  if (remaining_bits > 0)
  {
    word_type mask = (word_type{1} << remaining_bits) - 1;
    if ((m_data[full_words] & mask) != mask)
      return false;
  }
  return true;
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr bool bitvector<Word, Allocator>::none() const noexcept
{ return !any(); }

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::reset() noexcept
{
  for (size_type i = 0; i < m_capacity_words; ++i)
    m_data[i] = 0;
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::set_all() noexcept
{
  for (size_type i = 0; i < m_capacity_words; ++i)
    m_data[i] = std::numeric_limits<word_type>::max();

  // Mask unused bits in the last word
  size_type remaining = m_num_bits % bits_per_word;
  if (remaining > 0)
    m_data[m_capacity_words - 1] &= (word_type{1} << remaining) - 1;
}

template<word_c Word, word_allocator_c<Word> Allocator>
constexpr void bitvector<Word, Allocator>::flip_all() noexcept
{
  for (size_type i = 0; i < m_capacity_words; ++i)
    m_data[i] = ~m_data[i];

  // Mask unused bits in the last word
  size_type remaining = m_num_bits % bits_per_word;
  if (remaining > 0)
    m_data[m_capacity_words - 1] &= (word_type{1} << remaining) - 1;
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
