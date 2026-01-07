/*
 *  bitvector-core.cppm – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  lam.bitvector is a c++ module
 */

export module lam.bitvector:core;

import std;

namespace lam::bitvec
{

export struct uninitialized_t
{
};
export inline constexpr uninitialized_t uninitialized{};

export
template <typename T>
concept bitvector_allocator_c = std::same_as<typename T::value_type, std::uint64_t>;

export
template <bitvector_allocator_c Allocator = std::allocator<std::uint64_t>>
class bitvector
{
public:
  // Using uint64_t (64 bits) as the backing word type.
  using word_type = std::uint64_t;
  using size_type = std::size_t;
  using allocator_type = Allocator;

  constexpr allocator_type get_allocator() const noexcept { return m_alloc; }

  // Constants
  static constexpr size_type bits_per_word = std::numeric_limits<word_type>::digits;

  // Proxy class for operator[]
  class bit_proxy
  {
    friend class bitvector;

    word_type* m_word_ptr = nullptr;
    word_type m_mask = 0;

    constexpr bit_proxy(word_type* w, word_type mask) : m_word_ptr(w), m_mask(mask)
    { }

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
  constexpr bitvector(size_type num_bits, uninitialized_t, const Allocator& alloc = Allocator());
  
  template <std::ranges::input_range R>
  requires std::same_as<std::ranges::range_value_t<R>, word_type> && std::ranges::sized_range<R>
  constexpr explicit bitvector(R&& source, const Allocator& alloc = Allocator());

  constexpr bitvector();
  constexpr ~bitvector();

  constexpr bitvector(const bitvector& other);
  constexpr bitvector(bitvector&& other) noexcept;

  constexpr bitvector& operator=(const bitvector& other);
  constexpr bitvector& operator=(bitvector&& other) noexcept;

  [[nodiscard]] constexpr bool get(size_type index) const;
  constexpr bool operator[](size_type index) const;
  constexpr bit_proxy operator[](size_type index);

  constexpr void set(size_type index, bool value = true);
  constexpr void toggle(size_type index);
  constexpr void clear(size_type index);

  [[nodiscard]] constexpr size_type length() const;
  [[nodiscard]] constexpr size_type size() const;
  
  // Direct data access for optimized operations in other partitions
  [[nodiscard]] constexpr word_type* data() noexcept;
  [[nodiscard]] constexpr const word_type* data() const noexcept;
  [[nodiscard]] constexpr size_type word_count() const noexcept;

  // Utility methods
  [[nodiscard]] constexpr size_type count() const noexcept;  // popcount
  [[nodiscard]] constexpr bool any() const noexcept;
  [[nodiscard]] constexpr bool all() const noexcept;
  [[nodiscard]] constexpr bool none() const noexcept;
  constexpr void reset() noexcept;      // set all bits to 0
  constexpr void set_all() noexcept;    // set all bits to 1
  constexpr void flip_all() noexcept;   // invert all bits

  template <std::ranges::output_range<word_type> R>
  constexpr void export_words(R&& dest) const;

  constexpr void print() const;

  template <typename A, typename CharT>
  friend struct std::formatter;

  friend void swap(bitvector& a, bitvector& b) noexcept
  {
    using std::swap;
    swap(a.m_alloc, b.m_alloc);
    swap(a.m_data, b.m_data);
    swap(a.m_num_bits, b.m_num_bits);
    swap(a.m_capacity_words, b.m_capacity_words);
  }
};

// =============================================================================
// Implementation
// =============================================================================

// bit_proxy implementation

template <bitvector_allocator_c Allocator>

constexpr bitvector<Allocator>::bit_proxy& bitvector<Allocator>::bit_proxy::operator=(bool val)
{
  if (val)
    *m_word_ptr |= m_mask;
  else
    *m_word_ptr &= ~m_mask;
  return *this;
}

template <bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>::bit_proxy&
bitvector<Allocator>::bit_proxy::operator=(const bit_proxy& other)
{
  if (this == &other)
    return *this;
  return *this = static_cast<bool>(other);
}

template <bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>::bit_proxy::operator bool() const
{ return (*m_word_ptr & m_mask) != 0; }

template <bitvector_allocator_c Allocator>
constexpr bool bitvector<Allocator>::bit_proxy::operator~() const
{ return !static_cast<bool>(*this); }

template <bitvector_allocator_c Allocator>
constexpr void bitvector<Allocator>::bit_proxy::flip()
{ *m_word_ptr ^= m_mask; }

// bitvector implementation
template <bitvector_allocator_c Allocator>
constexpr typename bitvector<Allocator>::size_type
bitvector<Allocator>::words_needed(size_type num_bits) const
{ return (num_bits + bits_per_word - 1) / bits_per_word; }

template <bitvector_allocator_c Allocator>
constexpr void bitvector<Allocator>::allocate_storage(size_type num_bits, bool init_zeros)
{
  if (num_bits == 0)
  {
    m_data = nullptr;
    m_capacity_words = 0;
    return;
  }
  m_capacity_words = words_needed(num_bits);
  m_data = std::allocator_traits<Allocator>::allocate(m_alloc, m_capacity_words);
  
  if (init_zeros || std::is_constant_evaluated())
    for (size_type i = 0; i < m_capacity_words; ++i)
      std::allocator_traits<Allocator>::construct(m_alloc, m_data + i, 0);
}

template <bitvector_allocator_c Allocator>
constexpr void bitvector<Allocator>::deallocate_storage()
{
  if (m_data)
  {
    for (size_type i = 0; i < m_capacity_words; ++i)
      std::allocator_traits<Allocator>::destroy(m_alloc, m_data + i);
    std::allocator_traits<Allocator>::deallocate(m_alloc, m_data, m_capacity_words);
    m_data = nullptr;
    m_capacity_words = 0;
  }
}

template <bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>::bitvector(size_type num_bits, const Allocator& alloc)
    : m_alloc(alloc), 
      m_num_bits(num_bits)
{ allocate_storage(num_bits, true); }

template <bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>::bitvector(size_type num_bits, uninitialized_t, const Allocator& alloc)
    : m_alloc(alloc), 
      m_num_bits(num_bits)
{ allocate_storage(num_bits, false); }

template <bitvector_allocator_c Allocator>
template <std::ranges::input_range R>
requires std::same_as<std::ranges::range_value_t<R>, typename bitvector<Allocator>::word_type> && std::ranges::sized_range<R>
constexpr bitvector<Allocator>::bitvector(R&& source, const Allocator& alloc)
    : m_alloc(alloc)
{
  size_type num_words = std::ranges::size(source);
  m_num_bits = num_words * bits_per_word;
  allocate_storage(m_num_bits, false);

  if (m_data)
  {
    std::ranges::copy(std::forward<R>(source), m_data);
  }
}

template <bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>::bitvector() : bitvector(0) { }

template <bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>::~bitvector()
{ deallocate_storage(); }

template <bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>::bitvector(const bitvector& other)
    : m_alloc(std::allocator_traits<Allocator>::select_on_container_copy_construction(other.m_alloc)),
      m_num_bits(other.m_num_bits)
{
  allocate_storage(m_num_bits, false); // Don't zero, we copy immediately
  if (m_data)
    std::copy(other.m_data, other.m_data + m_capacity_words, m_data);
}

template <bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>::bitvector(bitvector&& other) noexcept
    : m_alloc(std::move(other.m_alloc)),
      m_data(other.m_data),
      m_num_bits(other.m_num_bits),
      m_capacity_words(other.m_capacity_words)
{
  other.m_data = nullptr;
  other.m_num_bits = 0;
  other.m_capacity_words = 0;
}

template <bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>& bitvector<Allocator>::operator=(const bitvector& other)
{
  if (this == &other)
    return *this;
  
  // Always propagate allocator first if POCCA is true
  if constexpr (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value)
  {
    if (m_alloc != other.m_alloc)
    {
      // Allocators differ and we must propagate - deallocate with old, use new
      deallocate_storage();
      m_alloc = other.m_alloc;
      m_num_bits = other.m_num_bits;
      allocate_storage(m_num_bits, false);
      if (m_data)
        std::copy(other.m_data, other.m_data + m_capacity_words, m_data);
      return *this;
    }
  }
  
  // Allocators are compatible, check if reallocation needed
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

template <bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>& bitvector<Allocator>::operator=(bitvector&& other) noexcept
{
  if (this == &other)
    return *this;

  deallocate_storage();

  // Move allocator propagation logic is complex, simplified here assuming compatible
  if constexpr (std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value)
    m_alloc = std::move(other.m_alloc);

  m_data = other.m_data;
  m_num_bits = other.m_num_bits;
  m_capacity_words = other.m_capacity_words;

  other.m_data = nullptr;
  other.m_num_bits = 0;
  other.m_capacity_words = 0;
  return *this;
}

template <bitvector_allocator_c Allocator>
constexpr bool bitvector<Allocator>::get(size_type index) const
{
  if (index >= m_num_bits)
    throw std::out_of_range("bitvector::get");
  return (m_data[index / bits_per_word] >> (index % bits_per_word)) & 1ULL;
}

template <bitvector_allocator_c Allocator>
constexpr bool bitvector<Allocator>::operator[](size_type index) const
{ return (m_data[index / bits_per_word] >> (index % bits_per_word)) & 1ULL; }

template <bitvector_allocator_c Allocator>
constexpr bitvector<Allocator>::bit_proxy bitvector<Allocator>::operator[](size_type index)
{ return bit_proxy(m_data + (index / bits_per_word), 1ULL << (index % bits_per_word)); }

template <bitvector_allocator_c Allocator>
constexpr void bitvector<Allocator>::set(size_type index, bool value)
{
  if (index >= m_num_bits)
    throw std::out_of_range("bitvector::set");
  if (value)
    m_data[index / bits_per_word] |= (1ULL << (index % bits_per_word));
  else
    m_data[index / bits_per_word] &= ~(1ULL << (index % bits_per_word));
}

template <bitvector_allocator_c Allocator>
constexpr void bitvector<Allocator>::toggle(size_type index)
{
  if (index >= m_num_bits)
    throw std::out_of_range("bitvector::toggle");
  m_data[index / bits_per_word] ^= (1ULL << (index % bits_per_word));
}

template <bitvector_allocator_c Allocator>
constexpr void bitvector<Allocator>::clear(size_type index)
{ set(index, false); }

template <bitvector_allocator_c Allocator>
constexpr typename bitvector<Allocator>::size_type bitvector<Allocator>::length() const
{ return m_num_bits; }

template <bitvector_allocator_c Allocator>
constexpr typename bitvector<Allocator>::size_type bitvector<Allocator>::size() const
{ return m_num_bits; }

template <bitvector_allocator_c Allocator>
constexpr typename bitvector<Allocator>::word_type* bitvector<Allocator>::data() noexcept
{ return m_data; }

template <bitvector_allocator_c Allocator>
constexpr const typename bitvector<Allocator>::word_type* bitvector<Allocator>::data() const noexcept
{ return m_data; }

template <bitvector_allocator_c Allocator>
constexpr typename bitvector<Allocator>::size_type bitvector<Allocator>::word_count() const noexcept
{ return m_capacity_words; }

// Utility method implementations

template <bitvector_allocator_c Allocator>
constexpr typename bitvector<Allocator>::size_type bitvector<Allocator>::count() const noexcept
{
  size_type total = 0;
  for (size_type i = 0; i < m_capacity_words; ++i)
    total += static_cast<size_type>(std::popcount(m_data[i]));
  return total;
}

template <bitvector_allocator_c Allocator>
constexpr bool bitvector<Allocator>::any() const noexcept
{
  for (size_type i = 0; i < m_capacity_words; ++i)
    if (m_data[i] != 0) 
      return true;
  return false;
}

template <bitvector_allocator_c Allocator>
constexpr bool bitvector<Allocator>::all() const noexcept
{
  if (m_num_bits == 0) 
    return true;
  
  // Check all complete words
  size_type full_words = m_num_bits / bits_per_word;
  for (size_type i = 0; i < full_words; ++i)
    if (m_data[i] != ~word_type{0}) 
      return false;
  
  // Check remaining bits in last word
  size_type remaining_bits = m_num_bits % bits_per_word;
  if (remaining_bits > 0)
  {
    word_type mask = (word_type{1} << remaining_bits) - 1;
    if ((m_data[full_words] & mask) != mask) 
      return false;
  }
  return true;
}

template <bitvector_allocator_c Allocator>
constexpr bool bitvector<Allocator>::none() const noexcept
{ return !any(); }

template <bitvector_allocator_c Allocator>
constexpr void bitvector<Allocator>::reset() noexcept
{
  for (size_type i = 0; i < m_capacity_words; ++i)
    m_data[i] = 0;
}

template <bitvector_allocator_c Allocator>
constexpr void bitvector<Allocator>::set_all() noexcept
{
  for (size_type i = 0; i < m_capacity_words; ++i)
    m_data[i] = ~word_type{0};

  // Mask unused bits in the last word
  size_type remaining = m_num_bits % bits_per_word;
  if (remaining > 0)
    m_data[m_capacity_words - 1] &= (word_type{1} << remaining) - 1;
}

template <bitvector_allocator_c Allocator>
constexpr void bitvector<Allocator>::flip_all() noexcept
{
  for (size_type i = 0; i < m_capacity_words; ++i)
    m_data[i] = ~m_data[i];

  // Mask unused bits in the last word
  size_type remaining = m_num_bits % bits_per_word;
  if (remaining > 0)
    m_data[m_capacity_words - 1] &= (word_type{1} << remaining) - 1;
}

template <bitvector_allocator_c Allocator>
constexpr void bitvector<Allocator>::print() const
{
  for (size_type i = 0; i < m_num_bits; ++i)
    std::print("{}", get(i) ? "1" : "0");
  std::println("");
}

template <bitvector_allocator_c Allocator>
template <std::ranges::output_range<typename bitvector<Allocator>::word_type> R>
constexpr void bitvector<Allocator>::export_words(R&& dest) const
{
  std::ranges::copy_n(m_data, m_capacity_words, std::ranges::begin(std::forward<R>(dest)));
}

} // namespace lam::bitvec

// Formatter specialization must be in global namespace, but we are in a module partition... 
// It should probably be in the primary interface or exported if possible.
// Standard practice is to keep it with the type.
template <typename Alloc>
struct std::formatter<lam::bitvec::bitvector<Alloc>> : std::formatter<std::string> // NOLINT(cert-dcl58-cpp)
{
  auto format(const lam::bitvec::bitvector<Alloc>& bv, std::format_context& ctx) const
  {
    std::string s;
    s.reserve(bv.size());
    for (std::size_t i = 0; i < bv.size(); ++i)
      s += bv[i] ? '1' : '0';
    return std::formatter<std::string>::format(s, ctx);
  }
};
