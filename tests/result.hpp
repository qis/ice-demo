#pragma once
#include "error.hpp"
#include <ice/result.hpp>
#include <fmt/core.h>
#include <type_traits>

struct flexible_type {
  int value{ 0 };

  constexpr flexible_type(int value) noexcept
    : value(value)
  {}

  flexible_type() = default;
  flexible_type(flexible_type&& other) = default;
  flexible_type(const flexible_type& other) = default;
  flexible_type& operator=(flexible_type&& other) = default;
  flexible_type& operator=(const flexible_type& other) = default;
};

struct no_default_constructible_type {
  int value;

  constexpr no_default_constructible_type(int value) noexcept
    : value(value)
  {}

  no_default_constructible_type() = delete;
  no_default_constructible_type(no_default_constructible_type&& other) = default;
  no_default_constructible_type(const no_default_constructible_type& other) = default;
  no_default_constructible_type& operator=(no_default_constructible_type&& other) = default;
  no_default_constructible_type& operator=(const no_default_constructible_type& other) = default;
};

struct no_move_constructible_type {
  int value{ 0 };

  constexpr no_move_constructible_type(int value) noexcept
    : value(value)
  {}

  no_move_constructible_type() = default;
  no_move_constructible_type(no_move_constructible_type&& other) = delete;
  no_move_constructible_type(const no_move_constructible_type& other) = default;
  no_move_constructible_type& operator=(no_move_constructible_type&& other) = default;
  no_move_constructible_type& operator=(const no_move_constructible_type& other) = default;
};

struct no_copy_constructible_type {
  int value{ 0 };

  constexpr no_copy_constructible_type(int value) noexcept
    : value(value)
  {}

  no_copy_constructible_type() = default;
  no_copy_constructible_type(no_copy_constructible_type&& other) = default;
  no_copy_constructible_type(const no_copy_constructible_type& other) = delete;
  no_copy_constructible_type& operator=(no_copy_constructible_type&& other) = default;
  no_copy_constructible_type& operator=(const no_copy_constructible_type& other) = default;
};

struct no_move_assignable_type {
  int value{ 0 };

  constexpr no_move_assignable_type(int value) noexcept
    : value(value)
  {}

  no_move_assignable_type() = default;
  no_move_assignable_type(no_move_assignable_type&& other) = default;
  no_move_assignable_type(const no_move_assignable_type& other) = default;
  no_move_assignable_type& operator=(no_move_assignable_type&& other) = delete;
  no_move_assignable_type& operator=(const no_move_assignable_type& other) = default;
};

struct no_copy_assignable_type {
  int value{ 0 };

  constexpr no_copy_assignable_type(int value) noexcept
    : value(value)
  {}

  no_copy_assignable_type() = default;
  no_copy_assignable_type(no_copy_assignable_type&& other) = default;
  no_copy_assignable_type(const no_copy_assignable_type& other) = default;
  no_copy_assignable_type& operator=(no_copy_assignable_type&& other) = default;
  no_copy_assignable_type& operator=(const no_copy_assignable_type& other) = delete;
};

struct restricted_type {
  int value{ 0 };

  constexpr restricted_type(int value) noexcept
    : value(value)
  {}

  restricted_type() = delete;
  restricted_type(restricted_type&& other) = default;
  restricted_type(const restricted_type& other) = delete;
  restricted_type& operator=(restricted_type&& other) = default;
  restricted_type& operator=(const restricted_type& other) = delete;
};

template <typename T>
struct fmt::formatter<ice::result<T>> : fmt::formatter<string_view> {
  template <typename FormatContext>
  auto format(const ice::result<T>& res, FormatContext& context) noexcept
  {
    if (res) {
      return fmt::formatter<string_view>::format("{value}", context);
    }
    const auto code = res.code();
    return fmt::formatter<string_view>::format(ice::error_text(res.type(), code), context);
    fmt::basic_memory_buffer<char, 16> buffer;
    fmt::format_to(buffer, "{:08X}", static_cast<unsigned>(code));
    return fmt::formatter<string_view>::format({ buffer.data(), buffer.size() }, context);
  }
};
