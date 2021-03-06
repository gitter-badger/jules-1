// Copyright (c) 2016 Filipe Verri <filipeverri@gmail.com>

#ifndef JULES_BASE_RANDOM_H
#define JULES_BASE_RANDOM_H

#include <jules/base/numeric.hpp>
#include <jules/core/meta.hpp>
#include <jules/core/range.hpp>

#include <cmath>
#include <random>
#include <vector>

namespace jules
{

static thread_local auto random_engine = std::mt19937{};

template <typename Dist> auto sample(Dist& dist) { return dist(random_engine); }

template <typename Dist> auto sample(index_t n, Dist& dist)
{
  auto rng = range::view::generate_n([&] { return dist(random_engine); }, n);
  auto compatible_rng = rng | range::view::bounded;
  return std::vector<bool>(range::begin(compatible_rng), range::end(compatible_rng));
}

static inline auto bernoulli_sample(index_t n, numeric p)
{
  auto dist = std::bernoulli_distribution(p);
  return sample(n, dist);
}

static inline auto bernoulli_sample(numeric p)
{
  auto dist = std::bernoulli_distribution(p);
  return sample(dist);
}

static inline auto uniform_index_sample(index_t end)
{
  auto dist = std::uniform_int_distribution<index_t>(0u, end - 1);
  return sample(dist);
}

template <typename Rng, typename U = range::range_value_t<Rng>, typename = meta::requires<range::Range<Rng>>>
auto discrete_index_sample(const Rng& rng)
{
  static_assert(std::is_convertible<U, double>::value, "weights must be convertible to double");
  auto dist = std::discrete_distribution<index_t>(range::begin(rng), range::end(rng));
  return sample(dist);
}

} // namespace jules

#endif // JULES_BASE_RANDOM_H
