#pragma once
#include <ice/format.hpp>
#include <cstdint>

enum invalid_type {};
enum class invalid_underlying_type : bool;
enum class invalid_underlying_type_size : std::int64_t;

enum class u16 : std::uint16_t;
enum class i32 : std::int32_t {
  success = 0,
  failure = -1,
};

using u16_error_code = ice::error_code<u16>;
using i32_error_code = ice::error_code<i32>;

constexpr ice::error_type u16_error_type = ice::make_error_type<u16>();
constexpr ice::error_type i32_error_type = ice::make_error_type<i32>();

constexpr u16_error_code u16_error_code_success = static_cast<u16>(0);
constexpr u16_error_code u16_error_code_failure = static_cast<u16>(1);

constexpr i32_error_code i32_error_code_success = i32::success;
constexpr i32_error_code i32_error_code_failure = i32::failure;
