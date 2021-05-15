#include "error_code.hpp"
#include "concepts.hpp"
#include "ice/application.hpp"
#include <algorithm>
#include <vector>
#include <cstdint>

// The [error code type] size must be 32-bit.
static_assert(sizeof(ice::error_type) == 4);

// The [error code] size must be 32-bit.
static_assert(sizeof(ice::error_code<u16>) == 4);

// The [error code type] must be an `enum class`.
static_assert(!ice::ErrorCodeType<invalid_type>);

// The [underlying error code type] must not be `bool`.
static_assert(!ice::ErrorCodeType<invalid_underlying_type>);

// The [underlying error code type] must not be larger, than 32-bit.
static_assert(!ice::ErrorCodeType<invalid_underlying_type_size>);

// Detect valid [error code types].
static_assert(ice::ErrorCodeType<u16>);
static_assert(ice::ErrorCodeType<i32>);

// Test `make_error_type<T>()` return type.
static_assert(std::is_same_v<ice::error_type, decltype(ice::make_error_type<u16>())>);
static_assert(std::is_same_v<ice::error_type, decltype(ice::make_error_type<i32>())>);

// Test `make_error_type<T>()` return value.
static_assert(u16_error_code_success.type() == ice::make_error_type<u16>());
static_assert(u16_error_code_failure.type() == ice::make_error_type<u16>());
static_assert(i32_error_code_success.type() == ice::make_error_type<i32>());
static_assert(i32_error_code_failure.type() == ice::make_error_type<i32>());

static_assert(u16_error_code_success.type() != ice::make_error_type<i32>());
static_assert(u16_error_code_failure.type() != ice::make_error_type<i32>());
static_assert(i32_error_code_success.type() != ice::make_error_type<u16>());
static_assert(i32_error_code_failure.type() != ice::make_error_type<u16>());

// Construction of an [error code] from its [error code type] is possible.
static_assert(std::is_constructible_v<u16_error_code, u16>);
static_assert(std::is_constructible_v<i32_error_code, i32>);

// Construction of an [error code] from any other type is not possible.
static_assert(!std::is_constructible_v<u16_error_code, i32>);
static_assert(!std::is_constructible_v<u16_error_code, invalid_type>);
static_assert(!std::is_constructible_v<u16_error_code, invalid_underlying_type>);
static_assert(!std::is_constructible_v<u16_error_code, invalid_underlying_type_size>);
static_assert(!std::is_constructible_v<u16_error_code, std::uint16_t>);
static_assert(!std::is_constructible_v<u16_error_code, bool>);

static_assert(!std::is_constructible_v<i32_error_code, u16>);
static_assert(!std::is_constructible_v<i32_error_code, invalid_type>);
static_assert(!std::is_constructible_v<i32_error_code, invalid_underlying_type>);
static_assert(!std::is_constructible_v<i32_error_code, invalid_underlying_type_size>);
static_assert(!std::is_constructible_v<i32_error_code, std::int32_t>);
static_assert(!std::is_constructible_v<i32_error_code, bool>);

// Conversion of [error code] objects with the same [error code type] is possible.
static_assert(std::is_convertible_v<u16_error_code, u16_error_code>);
static_assert(std::is_convertible_v<i32_error_code, i32_error_code>);

// Conversion of [error code] objects with different [error code type] is not possible.
static_assert(!std::is_convertible_v<u16_error_code, i32_error_code>);
static_assert(!std::is_convertible_v<i32_error_code, u16_error_code>);

// Conversion of an [error code type] to its [error code] is possible.
static_assert(std::is_convertible_v<u16, u16_error_code>);
static_assert(std::is_convertible_v<i32, i32_error_code>);

// Conversion between an [error code] and any other type is not possible.
static_assert(!std::is_convertible_v<u16_error_code, u16>);
static_assert(!std::is_convertible_v<i32_error_code, i32>);

static_assert(!std::is_convertible_v<u16_error_code, i32>);
static_assert(!std::is_convertible_v<u16_error_code, invalid_type>);
static_assert(!std::is_convertible_v<u16_error_code, invalid_underlying_type>);
static_assert(!std::is_convertible_v<u16_error_code, invalid_underlying_type_size>);
static_assert(!std::is_convertible_v<u16_error_code, std::uint16_t>);
static_assert(!std::is_convertible_v<u16_error_code, bool>);

static_assert(!std::is_convertible_v<i32_error_code, u16>);
static_assert(!std::is_convertible_v<i32_error_code, invalid_type>);
static_assert(!std::is_convertible_v<i32_error_code, invalid_underlying_type>);
static_assert(!std::is_convertible_v<i32_error_code, invalid_underlying_type_size>);
static_assert(!std::is_convertible_v<i32_error_code, std::int32_t>);
static_assert(!std::is_convertible_v<i32_error_code, bool>);

static_assert(!std::is_convertible_v<i32, u16_error_code>);
static_assert(!std::is_convertible_v<invalid_type, u16_error_code>);
static_assert(!std::is_convertible_v<invalid_underlying_type, u16_error_code>);
static_assert(!std::is_convertible_v<invalid_underlying_type_size, u16_error_code>);
static_assert(!std::is_convertible_v<std::uint16_t, u16_error_code>);
static_assert(!std::is_convertible_v<bool, u16_error_code>);

static_assert(!std::is_convertible_v<u16, i32_error_code>);
static_assert(!std::is_convertible_v<invalid_type, i32_error_code>);
static_assert(!std::is_convertible_v<invalid_underlying_type, i32_error_code>);
static_assert(!std::is_convertible_v<invalid_underlying_type_size, i32_error_code>);
static_assert(!std::is_convertible_v<std::int32_t, i32_error_code>);
static_assert(!std::is_convertible_v<bool, i32_error_code>);

// Comparison of [error code] objects.
static_assert(Comparable<u16_error_code>);
static_assert(Comparable<i32_error_code>);
static_assert(Comparable<u16_error_code, i32_error_code>);
static_assert(Comparable<i32_error_code, u16_error_code>);

static_assert(u16_error_code_success == ice::make_error_code<u16>(0));
static_assert(u16_error_code_failure == ice::make_error_code<u16>(1));
static_assert(i32_error_code_success == ice::make_error_code<i32>(0));
static_assert(i32_error_code_failure == ice::make_error_code<i32>(-1));

static_assert(u16_error_code_success != u16_error_code_failure);
static_assert(u16_error_code_success != i32_error_code_success);
static_assert(u16_error_code_success != i32_error_code_failure);

static_assert(u16_error_code_failure != u16_error_code_success);
static_assert(u16_error_code_failure != i32_error_code_success);
static_assert(u16_error_code_failure != i32_error_code_failure);

static_assert(i32_error_code_success != i32_error_code_failure);
static_assert(i32_error_code_success != u16_error_code_success);
static_assert(i32_error_code_success != u16_error_code_failure);

static_assert(i32_error_code_failure != i32_error_code_success);
static_assert(i32_error_code_failure != u16_error_code_success);
static_assert(i32_error_code_failure != u16_error_code_failure);

// Comparison of an [error code] and its [error code type].
static_assert(Comparable<u16_error_code, u16>);
static_assert(Comparable<i32_error_code, i32>);

static_assert(u16_error_code_success == static_cast<u16>(0));
static_assert(u16_error_code_failure == static_cast<u16>(1));
static_assert(u16_error_code_success != static_cast<u16>(1));
static_assert(u16_error_code_failure != static_cast<u16>(0));

static_assert(i32_error_code_success == i32::success);
static_assert(i32_error_code_failure == i32::failure);
static_assert(i32_error_code_success != i32::failure);
static_assert(i32_error_code_failure != i32::success);

static_assert(static_cast<u16>(0) == u16_error_code_success);
static_assert(static_cast<u16>(1) == u16_error_code_failure);
static_assert(static_cast<u16>(1) != u16_error_code_success);
static_assert(static_cast<u16>(0) != u16_error_code_failure);

static_assert(i32::success == i32_error_code_success);
static_assert(i32::failure == i32_error_code_failure);
static_assert(i32::failure != i32_error_code_success);
static_assert(i32::success != i32_error_code_failure);

// Comparison of an [error code] and any other type is not possible.
static_assert(NotComparable<u16_error_code, i32>);
static_assert(NotComparable<u16_error_code, invalid_type>);
static_assert(NotComparable<u16_error_code, invalid_underlying_type>);
static_assert(NotComparable<u16_error_code, invalid_underlying_type_size>);
static_assert(NotComparable<u16_error_code, std::uint16_t>);
static_assert(NotComparable<u16_error_code, bool>);

static_assert(NotComparable<i32_error_code, u16>);
static_assert(NotComparable<i32_error_code, invalid_type>);
static_assert(NotComparable<i32_error_code, invalid_underlying_type>);
static_assert(NotComparable<i32_error_code, invalid_underlying_type_size>);
static_assert(NotComparable<i32_error_code, std::int32_t>);
static_assert(NotComparable<i32_error_code, bool>);

// Ordering of [error code] objects.
static_assert(Ordered<u16_error_code>);
static_assert(Ordered<i32_error_code>);

static_assert(u16_error_code_success < u16_error_code_failure);
static_assert(u16_error_code_failure > u16_error_code_success);
static_assert(u16_error_code_success <= u16_error_code_failure);
static_assert(u16_error_code_failure >= u16_error_code_success);

static_assert(i32_error_code_success > i32_error_code_failure);
static_assert(i32_error_code_failure < i32_error_code_success);
static_assert(i32_error_code_success >= i32_error_code_failure);
static_assert(i32_error_code_failure <= i32_error_code_success);

// Ordering of [error code] and its [error code type].
static_assert(Ordered<u16_error_code, u16>);
static_assert(Ordered<i32_error_code, i32>);

static_assert(u16_error_code_success < static_cast<u16>(1));
static_assert(u16_error_code_failure > static_cast<u16>(0));
static_assert(u16_error_code_success <= static_cast<u16>(1));
static_assert(u16_error_code_failure >= static_cast<u16>(0));

static_assert(i32_error_code_success > i32::failure);
static_assert(i32_error_code_failure < i32::success);
static_assert(i32_error_code_success >= i32::failure);
static_assert(i32_error_code_failure <= i32::success);

static_assert(static_cast<u16>(1) > u16_error_code_success);
static_assert(static_cast<u16>(0) < u16_error_code_failure);
static_assert(static_cast<u16>(1) >= u16_error_code_success);
static_assert(static_cast<u16>(0) <= u16_error_code_failure);

static_assert(i32::failure < i32_error_code_success);
static_assert(i32::success > i32_error_code_failure);
static_assert(i32::failure <= i32_error_code_success);
static_assert(i32::success >= i32_error_code_failure);

// Ordering of [error code] and any other type is not possible.
static_assert(NotOrdered<u16_error_code, i32>);
static_assert(NotOrdered<u16_error_code, invalid_type>);
static_assert(NotOrdered<u16_error_code, invalid_underlying_type>);
static_assert(NotOrdered<u16_error_code, invalid_underlying_type_size>);
static_assert(NotOrdered<u16_error_code, std::uint16_t>);
static_assert(NotOrdered<u16_error_code, bool>);

static_assert(NotOrdered<i32_error_code, u16>);
static_assert(NotOrdered<i32_error_code, invalid_type>);
static_assert(NotOrdered<i32_error_code, invalid_underlying_type>);
static_assert(NotOrdered<i32_error_code, invalid_underlying_type_size>);
static_assert(NotOrdered<i32_error_code, std::int32_t>);
static_assert(NotOrdered<i32_error_code, bool>);

// Test `make_error_code<T>(T)` return type.
static_assert(std::is_same_v<ice::error_code<i32>, decltype(ice::make_error_code(i32::success))>);
static_assert(std::is_same_v<ice::error_code<i32>, decltype(ice::make_error_code(i32::failure))>);

// Test `make_error_code<T>(T)` return value.
static_assert(i32_error_code_success == ice::make_error_code(i32::success));
static_assert(i32_error_code_failure == ice::make_error_code(i32::failure));

// Test `make_error_code<T, U>(U)` return type.
static_assert(std::is_same_v<ice::error_code<u16>, decltype(ice::make_error_code<u16>(0))>);
static_assert(std::is_same_v<ice::error_code<u16>, decltype(ice::make_error_code<u16>(1))>);
static_assert(std::is_same_v<ice::error_code<i32>, decltype(ice::make_error_code<i32>(0))>);
static_assert(std::is_same_v<ice::error_code<i32>, decltype(ice::make_error_code<i32>(-1))>);

// Test `make_error_code<T, U>(U)` return value.
static_assert(u16_error_code_success == ice::make_error_code<u16>(0));
static_assert(u16_error_code_failure == ice::make_error_code<u16>(1));
static_assert(i32_error_code_success == ice::make_error_code<i32>(0));
static_assert(i32_error_code_failure == ice::make_error_code<i32>(-1));

#include <doctest/doctest.h>

namespace error_code_tests {

enum class errc;

ice::error_info make_error_info(errc)
{
  constexpr auto text = [](int code) -> std::string {
    switch (code) {
    case 0:
      return "operation succeeded";
    case 16:
      return "operation failed";
    }
    return "unknown";
  };
  return { "error code tests", text };
}

}  // namespace error_code_tests

TEST_CASE("error code with std::errc")
{
  ice::application application;

  constexpr ice::error_code<std::errc> success{};
  constexpr ice::error_code<std::errc> failure{ std::errc::no_such_file_or_directory };
  constexpr auto type = ice::make_error_type<std::errc>();

  CHECK(success.code() == 0);
  CHECK(success.type() == type);
  CHECK(fmt::format("{}", success) == "success");
  CHECK(fmt::format("{}", success.type()) == "system");

  CHECK(failure.code() == static_cast<int>(std::errc::no_such_file_or_directory));
  CHECK(failure.type() == type);
  CHECK(fmt::format("{}", failure) == "no such file or directory");
  CHECK(fmt::format("{}", failure.type()) == "system");
}

TEST_CASE("error code with error_code_tests::errc")
{
  ice::application application;

  constexpr ice::error_code<error_code_tests::errc> success{};
  constexpr ice::error_code<error_code_tests::errc> failure{ error_code_tests::errc{ 16 } };
  constexpr auto type = ice::make_error_type<error_code_tests::errc>();

  const auto code_number = fmt::format("{}", failure);
  CHECK(code_number.size() == 8);
  CHECK(code_number.find_first_not_of("1234567890ABCDEF") == std::string::npos);
  CHECK(code_number == "00000010");

  const auto type_number = fmt::format("{}", type);
  CHECK(type_number.size() == 8);
  CHECK(type_number.find_first_not_of("1234567890ABCDEF") == std::string::npos);
  CHECK(type_number != "00000000");

  CHECK(success.code() == 0);
  CHECK(success.type() == type);
  CHECK(fmt::format("{}", success) == "00000000");
  CHECK(fmt::format("{}", success.type()) == type_number);

  CHECK(failure.code() == 16);
  CHECK(failure.type() == type);
  CHECK(fmt::format("{}", failure) == code_number);
  CHECK(fmt::format("{}", failure.type()) == type_number);

  ice::load_error_info<error_code_tests::errc>();

  CHECK(success.code() == 0);
  CHECK(success.type() == type);
  CHECK(fmt::format("{}", success) == "operation succeeded");
  CHECK(fmt::format("{}", success.type()) == "error code tests");

  CHECK(failure.code() == 16);
  CHECK(failure.type() == type);
  CHECK(fmt::format("{}", failure) == "operation failed");
  CHECK(fmt::format("{}", failure.type()) == "error code tests");

  ice::unload_error_info<error_code_tests::errc>();

  CHECK(success.code() == 0);
  CHECK(success.type() == type);
  CHECK(fmt::format("{}", success) == "00000000");
  CHECK(fmt::format("{}", success.type()) == type_number);

  CHECK(failure.code() == 16);
  CHECK(failure.type() == type);
  CHECK(fmt::format("{}", failure) == code_number);
  CHECK(fmt::format("{}", failure.type()) == type_number);
}
