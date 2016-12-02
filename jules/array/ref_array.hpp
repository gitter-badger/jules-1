#ifndef JULES_ARRAY_REF_ARRAY_H
#define JULES_ARRAY_REF_ARRAY_H

#include <jules/array/detail/common.hpp>
#include <jules/array/detail/iterator.hpp>
#include <jules/array/detail/slicing.hpp>
#include <jules/array/slice.hpp>
#include <jules/core/debug.hpp>
#include <jules/core/range.hpp>
#include <jules/core/type.hpp>

namespace jules
{

/// Array reference.
///
/// This class is used internally by `jules` to represent a view of an concrete array.
///
/// \module N-Dimensional Array
template <typename T, std::size_t N> class ref_array
{
  static_assert(N > 0u, "Invalid array dimension.");

  template <typename, std::size_t> friend class ref_array;

public:
  using value_type = T;
  static constexpr auto order = N;

  using size_type = index_t;
  using difference_type = distance_t;

  using iterator = detail::iterator_from_indexes<T, typename base_slice<N>::iterator>;
  using const_iterator = detail::iterator_from_indexes<const T, typename base_slice<N>::iterator>;

  /// *TODO*: Explain why the user should probably not call this function.
  /// In C++17, we can provide a helper that generates a view with more security.
  ref_array(T* data, base_slice<N> descriptor) : data_{data}, descriptor_{descriptor} {}

  ref_array(const ref_array& source) = delete;
  ref_array(ref_array&& source) noexcept = delete;

  ~ref_array() = default;

  /// \group Assignment
  template <typename U> auto operator=(const U& source) -> detail::not_array_request<ref_array&, U>
  {
    static_assert(std::is_assignable<T&, U>::value, "incompatible assignment");
    for (auto& elem : *this)
      elem = source;
    return *this;
  }

  /// \group Assignment
  template <typename Array> auto operator=(const Array& source) -> detail::array_request<ref_array&, Array>
  {
    static_assert(Array::order == N, "array order mismatch");
    static_assert(std::is_assignable<T&, typename Array::value_type>::value, "incompatible assignment");

    DEBUG_ASSERT(this->extents() == source.extents(), debug::module{}, debug::level::extents_check, "extents mismatch");
    auto it = source.begin();
    for (auto& elem : *this)
      elem = *it++;
    DEBUG_ASSERT(it == source.end(), debug::module{}, debug::level::unreachable, "should never happen");

    return *this;
  }

  /// \group Assignment
  auto operator=(base_array<T, N>&& source) noexcept -> ref_array&
  {
    auto it = source.begin();
    for (auto& elem : *this)
      elem = std::move(*it++);
    return *this;
  }

  /// Implicitly convertable to hold const values.
  operator ref_array<const T, N>() const { return {data_, descriptor_}; }

  /// \group Indexing
  auto operator[](index_t i) -> ref_array<T, N - 1>
  {
    DEBUG_ASSERT(i <= row_count(), debug::module{}, debug::level::boundary_check, "out of range");

    const auto start = descriptor_.start + descriptor_.strides[0] * i;
    std::array<index_t, N - 1> extents, strides;

    const auto& e = extents();
    const auto& s = descriptor_.strides;

    std::copy(e.begin() + 1, e.end(), extents.begin());
    std::copy(s.begin() + 1, s.end(), strides.begin());

    return {data_, {start, extents, strides}};
  }

  /// \group Indexing
  auto operator[](index_t i) const -> ref_array<const T, N - 1> { return static_cast<ref_array<const T, N>>(*this)[i]; }

  template <typename... Args> auto operator()(Args&&... args) -> detail::indirect_request<ind_array<T, N>, Args...>
  {
    static_assert(sizeof...(args) == N, "invalid number of arguments");
    auto slicing = detail::indirect_slicing(descriptor_, std::forward<Args>(args)...);
    return {data_, slicing.first, std::move(slicing.second)};
  }

  template <typename... Args> auto operator()(Args&&... args) -> detail::slice_request<ref_array<T, N>, Args...>
  {
    static_assert(sizeof...(args) == N, "invalid number of arguments");
    auto slice = detail::default_slicing(descriptor_, std::forward<Args>(args)...);
    return {data_, slice};
  }

  template <typename... Args> auto operator()(Args&&... args) -> detail::element_request<T&, Args...>
  {
    static_assert(sizeof...(args) == N, "invalid number of arguments");
    // TODO: check boundaries
    return data_[descriptor_(index_t{std::forward<Args>(args)}...)];
  }

  template <typename... Args> decltype(auto) operator()(Args&&... args) const
  {
    return static_cast<ref_array<const T, N>>(*this)(std::forward<Args>(args)...);
  }

  auto begin() -> iterator { return {data_, descriptor_.begin()}; }
  auto end() -> iterator { return {data_, descriptor_.end()}; }

  auto begin() const -> const_iterator { return cbegin(); }
  auto end() const -> const_iterator { return cend(); }

  auto cbegin() const -> const_iterator { return {data_, descriptor_.begin()}; }
  auto cend() const -> const_iterator { return {data_, descriptor_.end()}; }

  auto descriptor() const { return descriptor_; }
  auto data() const { return data_; }
  auto extents() const { return descriptor_.extents; }
  auto size() const { return descriptor_.size(); }

  auto row_count() const { return extents()[1]; }
  auto column_count() const { return extents()[2]; }

protected:
  T* data_;
  base_slice<N> descriptor_;
};

/// 1-D Array reference specialization.
///
/// This class is used internally by `jules` to represent a view of an concrete array.
///
/// \module N-Dimensional Array
template <typename T> class ref_array<T, 1>
{
  template <typename, std::size_t> friend class ref_array;

public:
  using value_type = T;
  static constexpr auto order = 1;

  using size_type = index_t;
  using difference_type = distance_t;

  using iterator = detail::iterator_from_indexes<T, typename base_slice<1>::iterator>;
  using const_iterator = detail::iterator_from_indexes<const T, typename base_slice<1>::iterator>;

  /// *TODO*: Explain why the user should probably not call this function.
  /// In C++17, we can provide a helper that generates a view with more security.
  ref_array(T* data, base_slice<1> descriptor) : data_{data}, descriptor_{descriptor} {}

  ref_array(const ref_array& source) = delete;
  ref_array(ref_array&& source) noexcept = delete;

  ~ref_array() = default;

  /// \group Assignment
  template <typename U> auto operator=(const U& source) -> detail::not_array_request<ref_array&, U>
  {
    static_assert(std::is_assignable<T&, U>::value, "incompatible assignment");
    for (auto& elem : *this)
      elem = source;
    return *this;
  }

  /// \group Assignment
  template <typename Array> auto operator=(const Array& source) -> detail::array_request<ref_array&, Array>
  {
    static_assert(Array::order == 1, "array order mismatch");
    static_assert(std::is_assignable<T&, typename Array::value_type>::value, "incompatible assignment");

    DEBUG_ASSERT(this->extents() == source.extents(), debug::module{}, debug::level::extents_check, "extents mismatch");
    auto it = source.begin();
    for (auto& elem : *this)
      elem = *it++;
    DEBUG_ASSERT(it == source.end(), debug::module{}, debug::level::unreachable, "should never happen");

    return *this;
  }

  /// \group Assignment
  auto operator=(base_array<T, 1>&& source) noexcept -> ref_array&
  {
    auto it = source.begin();
    for (auto& elem : *this)
      elem = std::move(*it++);
    return *this;
  }

  /// Implicitly convertable to hold const values.
  operator ref_array<const T, 1>() const { return {data_, descriptor_}; }

  /// \group Indexing
  auto operator[](index_t i) -> T&
  {
    DEBUG_ASSERT(i <= length(), debug::module{}, debug::level::boundary_check, "out of range");
    return data_[descriptor_(i)];
  }

  /// \group Indexing
  auto operator[](index_t i) const -> const T&
  {
    DEBUG_ASSERT(i <= length(), debug::module{}, debug::level::boundary_check, "out of range");
    return data_[descriptor_(i)];
  }

  template <typename Rng, typename U = range::range_value_t<Rng>, CONCEPT_REQUIRES_(range::Range<Rng>())>
  auto operator()(const Rng& rng) -> ind_array<T, 1>
  {
    static_assert(std::is_convertible<U, index_t>::value, "arbitrary ranges must contain indexes");
    auto slicing = detail::indirect_slicing(this->descriptor_, rng);
    return {data_, slicing.first, std::move(slicing.second)};
  }

  auto operator()(const base_slice<1>& slice) -> ref_array<T, 1>
  {
    auto new_slice = default_slicing(this->descriptor_, slice);
    return {data_, new_slice};
  }

  auto operator()(index_t i) -> T& { return (*this)[i]; }

  template <typename Arg> decltype(auto) operator()(Arg&& arg) const
  {
    return static_cast<ref_array<const T, 1>>(*this)(std::forward<Arg>(arg));
  }

  auto begin() -> iterator { return {data_, descriptor_.begin()}; }
  auto end() -> iterator { return {data_, descriptor_.end()}; }

  auto begin() const -> const_iterator { return cbegin(); }
  auto end() const -> const_iterator { return cend(); }

  auto cbegin() const -> const_iterator { return {data_, descriptor_.begin()}; }
  auto cend() const -> const_iterator { return {data_, descriptor_.end()}; }

  auto descriptor() const { return descriptor_; }
  auto data() const { return data_; }
  auto extents() const { return descriptor_.extent; }
  auto size() const { return descriptor_.size(); }
  auto length() const { return size(); }

protected:
  T* data_;
  base_slice<1> descriptor_;
};

} // namespace jules

#endif // JULES_ARRAY_REF_ARRAY_H