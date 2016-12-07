#include "jules/array/all.hpp"

#include <functional>
#include <iostream> // TODO
#include <type_traits>

#include <catch.hpp>

template <typename T, typename U> constexpr auto same_type(const T&, const U&) { return std::is_same<T, U>::value; }
template <typename T, typename U> constexpr auto is(const U&) { return std::is_same<T, U>::value; }

TEST_CASE("Vector tutorial", "[vector]")
{
  SECTION("Constructors")
  {
    // Default constructor.
    auto empty = jules::vector<>();
    CHECK(empty.size() == 0u);
    CHECK(empty.data() == nullptr);
    CHECK(empty.begin() == empty.end());
    CHECK(empty.cbegin() == empty.cend());

    // Constructors with size and optional default value.
    auto all_same1 = jules::vector<long>(3l, 10u);
    auto all_same2 = jules::vector<long>(10u);
    all_same2() = 3l;

    CHECK(all(all_same1 == 3l));
    CHECK(all(all_same2 == 3l));
    CHECK(all(all_same1 == all_same2));

    auto values = std::array<long, 10>{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

    auto a = jules::vector<long>(10u);
    auto b = jules::vector<long>(10u);

    jules::range::copy(values, a.begin());
    b() = a;

    // Constructors from iterators and ranges.
    auto c = jules::vector<long>(std::begin(values), std::end(values));
    auto d = jules::vector<long>(values);

    // Copy and move constructors.
    auto e = [d]() -> jules::vector<long> { return {std::move(d)}; }();

    REQUIRE(all(a == b));
    REQUIRE(all(b == c));
    REQUIRE(all(c == d));
    REQUIRE(all(d == e));

    // Constructors from slicing.
    auto x = jules::vector<long>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto even = jules::vector<long>(x(jules::slice(0u, jules::slice::all, 2u)));
    auto odd = jules::vector<long>(x(jules::seq(1u, x.length(), 2u)));

    CHECK(all(x == jules::as_vector(0, 1, 2, 3, 4, 5, 6, 7, 8, 9)));
    CHECK(all(x == jules::as_vector(values)));
    CHECK(all(even == jules::as_vector(0, 2, 4, 6, 8)));
    CHECK(all(odd == jules::as_vector(1, 3, 5, 7, 9)));

    // Constructors from expression.
    auto y = jules::vector<long>(x + x(jules::seq(x.length(), 0u) - 1u));
    CHECK(all(y == 9));
  }

  SECTION("Assignments")
  {
    auto x = jules::vector<long>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto y = jules::vector<long>(-20l, 20u);
    auto z = jules::vector<long>(-10l, 5u);

    y = x;
    CHECK(all(x == y));

    z = std::move(x);
    CHECK(all(y == z));

    x = z(jules::slice(0u, jules::slice::all, 2u));
    CHECK(all(x == jules::as_vector(0, 2, 4, 6, 8)));

    x = z(jules::seq(0u, z.length(), 2u));
    CHECK(all(x == jules::as_vector(0, 2, 4, 6, 8)));

    z = x + x;
    CHECK(all(z == jules::as_vector(0, 4, 8, 12, 16)));

    x = eval(x + x);
    CHECK(all(x == jules::as_vector(0, 4, 8, 12, 16)));

    // This one is tricky. We must guarantee that the Memory of x is not freed before the
    // expression is evaluated.
    x = x - x;
    CHECK(all(x == jules::as_vector(jules::repeat<5>(0))));
  }
}
