#pragma once
#include <ice/application.hpp>
#include <concepts>
#include <type_traits>

// clang-format off

template <typename T, typename U>
concept LeftEqualRight = requires(const std::remove_reference_t<T>& lhs, const std::remove_reference_t<U>& rhs)
{
  { lhs == rhs } -> std::same_as<bool>;
};

template <typename T, typename U>
concept LeftNotEqualRight = requires(const std::remove_reference_t<T>& lhs, const std::remove_reference_t<U>& rhs)
{
  { lhs != rhs } -> std::same_as<bool>;
};

template <typename T, typename U>
concept RightEqualLeft = requires(const std::remove_reference_t<T>& lhs, const std::remove_reference_t<U>& rhs)
{
  { rhs == lhs } -> std::same_as<bool>;
};

template <typename T, typename U>
concept RightNotEqualLeft = requires(const std::remove_reference_t<T>& lhs, const std::remove_reference_t<U>& rhs)
{
  { rhs != lhs } -> std::same_as<bool>;
};

// clang-format on

template <typename T, typename U = T>
concept Comparable = requires(T t, U u)
{
  requires(LeftEqualRight<T, U>);
  requires(LeftNotEqualRight<T, U>);
  requires(RightEqualLeft<T, U>);
  requires(RightNotEqualLeft<T, U>);
};

template <typename T, typename U = T>
concept NotComparable = requires(T, U u)
{
  requires(!LeftEqualRight<T, U>);
  requires(!LeftNotEqualRight<T, U>);
  requires(!RightEqualLeft<T, U>);
  requires(!RightNotEqualLeft<T, U>);
};

// clang-format off

template <typename T, typename U>
concept LeftGreaterRight = requires(const std::remove_reference_t<T>& lhs, const std::remove_reference_t<U>& rhs)
{
  { lhs > rhs } -> std::same_as<bool>;
};

template <typename T, typename U>
concept LeftLesserRight = requires(const std::remove_reference_t<T>& lhs, const std::remove_reference_t<U>& rhs)
{
  { lhs < rhs } -> std::same_as<bool>;
};

template <typename T, typename U>
concept LeftGreaterEqualRight = requires(const std::remove_reference_t<T>& lhs, const std::remove_reference_t<U>& rhs)
{
  { lhs >= rhs } -> std::same_as<bool>;
};

template <typename T, typename U>
concept LeftLesserEqualRight = requires(const std::remove_reference_t<T>& lhs, const std::remove_reference_t<U>& rhs)
{
  { lhs <= rhs } -> std::same_as<bool>;
};

template <typename T, typename U>
concept RightGreaterLeft = requires(const std::remove_reference_t<T>& lhs, const std::remove_reference_t<U>& rhs) {
  { rhs > lhs } -> std::same_as<bool>;
};

template <typename T, typename U>
concept RightLesserLeft = requires(const std::remove_reference_t<T>& lhs, const std::remove_reference_t<U>& rhs)
{
  { rhs < lhs } -> std::same_as<bool>;
};

template <typename T, typename U>
concept RightGreaterEqualLeft = requires(const std::remove_reference_t<T>& lhs, const std::remove_reference_t<U>& rhs)
{
  { rhs >= lhs } -> std::same_as<bool>;
};

template <typename T, typename U>
concept RightLesserEqualLeft = requires(const std::remove_reference_t<T>& lhs, const std::remove_reference_t<U>& rhs)
{
  { rhs <= lhs } -> std::same_as<bool>;
};

// clang-format on

template <typename T, typename U = T>
concept Ordered = requires(T, U u)
{
  requires(LeftGreaterRight<T, U>);
  requires(LeftLesserRight<T, U>);
  requires(LeftGreaterEqualRight<T, U>);
  requires(LeftLesserEqualRight<T, U>);
  requires(RightGreaterLeft<T, U>);
  requires(RightLesserLeft<T, U>);
  requires(RightGreaterEqualLeft<T, U>);
  requires(RightLesserEqualLeft<T, U>);
};

template <typename T, typename U = T>
concept NotOrdered = requires(T, U u)
{
  requires(!LeftGreaterRight<T, U>);
  requires(!LeftLesserRight<T, U>);
  requires(!LeftGreaterEqualRight<T, U>);
  requires(!LeftLesserEqualRight<T, U>);
  requires(!RightGreaterLeft<T, U>);
  requires(!RightLesserLeft<T, U>);
  requires(!RightGreaterEqualLeft<T, U>);
  requires(!RightLesserEqualLeft<T, U>);
};

template <typename T>
concept Sortable = requires(T)
{
  requires(LeftLesserRight<T, T>);
};

template <typename T>
concept NotSortable = requires(T)
{
  requires(!LeftLesserRight<T, T>);
};
