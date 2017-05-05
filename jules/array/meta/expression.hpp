// Copyright (c) 2017 Filipe Verri <filipeverri@gmail.com>

#ifndef JULES_ARRAY_META_EXPRESSION_H
/// \exclude
#define JULES_ARRAY_META_EXPRESSION_H

#include <jules/array/meta/common.hpp>
#include <jules/core/meta.hpp>
#include <jules/core/range.hpp>

namespace jules
{

template <typename T, typename = void> struct ArrayExpression : std::false_type {
};

template <typename T>
struct ArrayExpression<                        //
  T, meta::requires<                           //
       std::negation<DefaultConstructible<T>>, //
       std::negation<CopyConstructible<T>>,    //
       std::negation<MoveConstructible<T>>,    //
       std::negation<Copyable<T>>,             //
       std::negation<Movable<T>>,              //
       range::InputRange<T>,                   //
       CommonArray<T>                          //
       >> : std::true_type {
};

} // namespace jules

#endif // JULES_ARRAY_META_EXPRESSION_H
