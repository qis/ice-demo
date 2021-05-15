#include "error.hpp"
#include "concepts.hpp"
#include <algorithm>
#include <vector>
#include <cstdint>

// The [error] size must be 64-bit.
static_assert(sizeof(ice::error) == 8);

// Construction of an [error] from an [error code] is possible.
static_assert(std::is_constructible_v<ice::error, u16_error_code>);
static_assert(std::is_constructible_v<ice::error, i32_error_code>);

// Construction of an [error] from an [error code type] is possible.
static_assert(std::is_constructible_v<ice::error, u16>);
static_assert(std::is_constructible_v<ice::error, i32>);

// Construction of an [error] from any other type is not possible.
static_assert(!std::is_constructible_v<ice::error, invalid_type>);
static_assert(!std::is_constructible_v<ice::error, invalid_underlying_type>);
static_assert(!std::is_constructible_v<ice::error, invalid_underlying_type_size>);
static_assert(!std::is_constructible_v<ice::error, std::uint16_t>);
static_assert(!std::is_constructible_v<ice::error, bool>);

// Conversion between [error] objects is possible.
static_assert(std::is_convertible_v<ice::error, ice::error>);

// Conversion of an [error] to any other type is not possible.
static_assert(!std::is_convertible_v<ice::error, u16>);
static_assert(!std::is_convertible_v<ice::error, i32>);
static_assert(!std::is_convertible_v<ice::error, u16_error_code>);
static_assert(!std::is_convertible_v<ice::error, i32_error_code>);
static_assert(!std::is_convertible_v<ice::error, invalid_type>);
static_assert(!std::is_convertible_v<ice::error, invalid_underlying_type>);
static_assert(!std::is_convertible_v<ice::error, invalid_underlying_type_size>);
static_assert(!std::is_convertible_v<ice::error, std::uint16_t>);
static_assert(!std::is_convertible_v<ice::error, bool>);

// Conversion of [error code] to [error] is possible.
static_assert(std::is_convertible_v<u16_error_code, ice::error>);
static_assert(std::is_convertible_v<i32_error_code, ice::error>);

// Conversion of [error code type] to [error] is possible.
static_assert(std::is_convertible_v<u16, ice::error>);
static_assert(std::is_convertible_v<i32, ice::error>);

// Conversion of any other type to [error] is not possible.
static_assert(!std::is_convertible_v<invalid_type, ice::error>);
static_assert(!std::is_convertible_v<invalid_underlying_type, ice::error>);
static_assert(!std::is_convertible_v<invalid_underlying_type_size, ice::error>);
static_assert(!std::is_convertible_v<std::uint16_t, ice::error>);
static_assert(!std::is_convertible_v<bool, ice::error>);

// Comparison of [error] objects.
static_assert(Comparable<ice::error>);

static_assert(u16_error_success == ice::make_error<u16>(0));
static_assert(u16_error_failure == ice::make_error<u16>(1));
static_assert(i32_error_success == ice::make_error<i32>(0));
static_assert(i32_error_failure == ice::make_error<i32>(-1));

static_assert(u16_error_success != u16_error_failure);
static_assert(u16_error_success != i32_error_success);
static_assert(u16_error_success != i32_error_failure);

static_assert(u16_error_failure != u16_error_success);
static_assert(u16_error_failure != i32_error_success);
static_assert(u16_error_failure != i32_error_failure);

static_assert(i32_error_success != i32_error_failure);
static_assert(i32_error_success != u16_error_success);
static_assert(i32_error_success != u16_error_failure);

static_assert(i32_error_failure != i32_error_success);
static_assert(i32_error_failure != u16_error_success);
static_assert(i32_error_failure != u16_error_failure);

// Comparison of [error] and [error code].
static_assert(Comparable<ice::error, u16_error_code>);
static_assert(Comparable<ice::error, i32_error_code>);

static_assert(u16_error_success == u16_error_code_success);
static_assert(u16_error_failure == u16_error_code_failure);
static_assert(i32_error_success == i32_error_code_success);
static_assert(i32_error_failure == i32_error_code_failure);

static_assert(u16_error_success != u16_error_code_failure);
static_assert(u16_error_success != i32_error_code_success);
static_assert(u16_error_success != i32_error_code_failure);

static_assert(u16_error_failure != u16_error_code_success);
static_assert(u16_error_failure != i32_error_code_success);
static_assert(u16_error_failure != i32_error_code_failure);

static_assert(i32_error_success != i32_error_code_failure);
static_assert(i32_error_success != u16_error_code_success);
static_assert(i32_error_success != u16_error_code_failure);

static_assert(i32_error_failure != i32_error_code_success);
static_assert(i32_error_failure != u16_error_code_success);
static_assert(i32_error_failure != u16_error_code_failure);

static_assert(u16_error_code_success == u16_error_success);
static_assert(u16_error_code_failure == u16_error_failure);
static_assert(i32_error_code_success == i32_error_success);
static_assert(i32_error_code_failure == i32_error_failure);

static_assert(u16_error_code_success != u16_error_failure);
static_assert(u16_error_code_success != i32_error_success);
static_assert(u16_error_code_success != i32_error_failure);

static_assert(u16_error_code_failure != u16_error_success);
static_assert(u16_error_code_failure != i32_error_success);
static_assert(u16_error_code_failure != i32_error_failure);

static_assert(i32_error_code_success != i32_error_failure);
static_assert(i32_error_code_success != u16_error_success);
static_assert(i32_error_code_success != u16_error_failure);

static_assert(i32_error_code_failure != i32_error_success);
static_assert(i32_error_code_failure != u16_error_success);
static_assert(i32_error_code_failure != u16_error_failure);

// Comparison of [error] and [error code type].
static_assert(Comparable<ice::error, u16>);
static_assert(Comparable<ice::error, i32>);

static_assert(u16_error_success == static_cast<u16>(0));
static_assert(u16_error_failure == static_cast<u16>(1));
static_assert(i32_error_success == i32::success);
static_assert(i32_error_failure == i32::failure);

static_assert(u16_error_success != static_cast<u16>(1));
static_assert(u16_error_success != i32::success);
static_assert(u16_error_success != i32::failure);

static_assert(u16_error_failure != static_cast<u16>(0));
static_assert(u16_error_failure != i32::success);
static_assert(u16_error_failure != i32::failure);

static_assert(i32_error_success != i32::failure);
static_assert(i32_error_success != static_cast<u16>(0));
static_assert(i32_error_success != static_cast<u16>(1));

static_assert(i32_error_failure != i32::success);
static_assert(i32_error_failure != static_cast<u16>(0));
static_assert(i32_error_failure != static_cast<u16>(1));

static_assert(static_cast<u16>(0) == u16_error_success);
static_assert(static_cast<u16>(1) == u16_error_failure);
static_assert(i32::success == i32_error_success);
static_assert(i32::failure == i32_error_failure);

static_assert(static_cast<u16>(0) != u16_error_failure);
static_assert(static_cast<u16>(0) != i32_error_success);
static_assert(static_cast<u16>(0) != i32_error_failure);

static_assert(static_cast<u16>(1) != u16_error_success);
static_assert(static_cast<u16>(1) != i32_error_success);
static_assert(static_cast<u16>(1) != i32_error_failure);

static_assert(i32::success != i32_error_failure);
static_assert(i32::success != u16_error_success);
static_assert(i32::success != u16_error_failure);

static_assert(i32::failure != i32_error_success);
static_assert(i32::failure != u16_error_success);
static_assert(i32::failure != u16_error_failure);

// Comparison of [error] and any other type is not possible.
static_assert(NotComparable<ice::error, invalid_type>);
static_assert(NotComparable<ice::error, invalid_underlying_type>);
static_assert(NotComparable<ice::error, invalid_underlying_type_size>);
static_assert(NotComparable<ice::error, std::uint16_t>);
static_assert(NotComparable<ice::error, bool>);

// Ordering of [error] objects.
static_assert(Ordered<ice::error>);

static_assert(i32_error_type > u16_error_type ? ice::make_error<i32>(1) > ice::make_error<u16>(2)
                                              : ice::make_error<i32>(2) < ice::make_error<u16>(1));

static_assert(u16_error_success < u16_error_failure);
static_assert(u16_error_failure > u16_error_success);
static_assert(u16_error_success <= u16_error_failure);
static_assert(u16_error_failure >= u16_error_success);

static_assert(i32_error_success > i32_error_failure);
static_assert(i32_error_failure < i32_error_success);
static_assert(i32_error_success >= i32_error_failure);
static_assert(i32_error_failure <= i32_error_success);

// Ordering of [error] and [error code].
static_assert(Ordered<ice::error, u16_error_code>);
static_assert(Ordered<ice::error, i32_error_code>);

static_assert(i32_error_type > u16_error_type ? ice::make_error<i32>(1) > ice::make_error_code<u16>(2)
                                              : ice::make_error<i32>(2) < ice::make_error_code<u16>(1));

static_assert(i32_error_type > u16_error_type ? ice::make_error_code<i32>(1) > ice::make_error<u16>(2)
                                              : ice::make_error_code<i32>(2) < ice::make_error<u16>(1));

static_assert(u16_error_success < u16_error_code_failure);
static_assert(u16_error_failure > u16_error_code_success);
static_assert(u16_error_success <= u16_error_code_failure);
static_assert(u16_error_failure >= u16_error_code_success);

static_assert(i32_error_success > i32_error_code_failure);
static_assert(i32_error_failure < i32_error_code_success);
static_assert(i32_error_success >= i32_error_code_failure);
static_assert(i32_error_failure <= i32_error_code_success);

static_assert(u16_error_code_success < u16_error_failure);
static_assert(u16_error_code_failure > u16_error_success);
static_assert(u16_error_code_success <= u16_error_failure);
static_assert(u16_error_code_failure >= u16_error_success);

static_assert(i32_error_code_success > i32_error_failure);
static_assert(i32_error_code_failure < i32_error_success);
static_assert(i32_error_code_success >= i32_error_failure);
static_assert(i32_error_code_failure <= i32_error_success);

// Ordering of [error] and [error code type].
static_assert(Ordered<ice::error, u16>);
static_assert(Ordered<ice::error, i32>);

static_assert(i32_error_type > u16_error_type ? ice::make_error<i32>(1) > static_cast<u16>(2)
                                              : ice::make_error<i32>(2) < static_cast<u16>(1));

static_assert(i32_error_type > u16_error_type ? static_cast<i32>(1) > ice::make_error<u16>(2)
                                              : static_cast<i32>(2) < ice::make_error<u16>(1));

static_assert(u16_error_success < static_cast<u16>(1));
static_assert(u16_error_failure > static_cast<u16>(0));
static_assert(u16_error_success <= static_cast<u16>(1));
static_assert(u16_error_failure >= static_cast<u16>(0));

static_assert(i32_error_success > i32::failure);
static_assert(i32_error_failure < i32::success);
static_assert(i32_error_success >= i32::failure);
static_assert(i32_error_failure <= i32::success);

static_assert(static_cast<u16>(0) < u16_error_failure);
static_assert(static_cast<u16>(1) > u16_error_success);
static_assert(static_cast<u16>(0) <= u16_error_failure);
static_assert(static_cast<u16>(1) >= u16_error_success);

static_assert(i32::success > i32_error_failure);
static_assert(i32::failure < i32_error_success);
static_assert(i32::success >= i32_error_failure);
static_assert(i32::failure <= i32_error_success);

// Ordering of [error] and any other type is not possible.
static_assert(NotOrdered<ice::error, invalid_type>);
static_assert(NotOrdered<ice::error, invalid_underlying_type>);
static_assert(NotOrdered<ice::error, invalid_underlying_type_size>);
static_assert(NotOrdered<ice::error, std::uint16_t>);
static_assert(NotOrdered<ice::error, bool>);

// Test `make_error<T>(T)` return type.
static_assert(std::is_same_v<ice::error, decltype(ice::make_error(i32::success))>);
static_assert(std::is_same_v<ice::error, decltype(ice::make_error(i32::failure))>);

// Test `make_error<T>(T)` return value.
static_assert(i32_error_success == ice::make_error(i32::success));
static_assert(i32_error_failure == ice::make_error(i32::failure));

// Test `make_error<T, U>(U)` return type.
static_assert(std::is_same_v<ice::error, decltype(ice::make_error<u16>(0))>);
static_assert(std::is_same_v<ice::error, decltype(ice::make_error<u16>(1))>);
static_assert(std::is_same_v<ice::error, decltype(ice::make_error<i32>(0))>);
static_assert(std::is_same_v<ice::error, decltype(ice::make_error<i32>(-1))>);

// Test `make_error<T, U>(U)` return value.
static_assert(u16_error_success == ice::make_error<u16>(0));
static_assert(u16_error_failure == ice::make_error<u16>(1));
static_assert(i32_error_success == ice::make_error<i32>(0));
static_assert(i32_error_failure == ice::make_error<i32>(-1));

#include <doctest/doctest.h>

namespace error_tests {

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
  return { "error tests", text };
}

}  // namespace error_tests

TEST_CASE("error with std::errc")
{
  ice::application application;

  constexpr ice::error success{};
  constexpr ice::error failure{ std::errc::no_such_file_or_directory };
  constexpr auto type = ice::make_error_type<std::errc>();

  CHECK(success.code() == 0);
  CHECK(success.type() == ice::error_type::success);
  CHECK(fmt::format("{}", success) == "00000000");
  CHECK(fmt::format("{}", success.type()) == "00000000");

  CHECK(failure.code() == static_cast<int>(std::errc::no_such_file_or_directory));
  CHECK(failure.type() == type);
  CHECK(fmt::format("{}", failure) == "no such file or directory");
  CHECK(fmt::format("{}", failure.type()) == "system");
}

TEST_CASE("error with error_tests::errc")
{
  ice::application application;

  constexpr ice::error success{};
  constexpr ice::error failure{ error_tests::errc{ 16 } };
  constexpr auto type = ice::make_error_type<error_tests::errc>();

  const auto code_number = fmt::format("{}", failure);
  CHECK(code_number.size() == 8);
  CHECK(code_number.find_first_not_of("1234567890ABCDEF") == std::string::npos);
  CHECK(code_number == "00000010");

  const auto type_number = fmt::format("{}", type);
  CHECK(type_number.size() == 8);
  CHECK(type_number.find_first_not_of("1234567890ABCDEF") == std::string::npos);
  CHECK(type_number != "00000000");

  CHECK(success.code() == 0);
  CHECK(success.type() == ice::error_type::success);
  CHECK(fmt::format("{}", success) == "00000000");
  CHECK(fmt::format("{}", success.type()) == "00000000");

  CHECK(failure.code() == 16);
  CHECK(failure.type() == type);
  CHECK(fmt::format("{}", failure) == code_number);
  CHECK(fmt::format("{}", failure.type()) == type_number);

  ice::load_error_info<error_tests::errc>();

  CHECK(success.code() == 0);
  CHECK(success.type() == ice::error_type::success);
  CHECK(fmt::format("{}", success) == "00000000");
  CHECK(fmt::format("{}", success.type()) == "00000000");

  CHECK(failure.code() == 16);
  CHECK(failure.type() == type);
  CHECK(fmt::format("{}", failure) == "operation failed");
  CHECK(fmt::format("{}", failure.type()) == "error tests");

  ice::unload_error_info<error_tests::errc>();

  CHECK(success.code() == 0);
  CHECK(success.type() == ice::error_type::success);
  CHECK(fmt::format("{}", success) == "00000000");
  CHECK(fmt::format("{}", success.type()) == "00000000");

  CHECK(failure.code() == 16);
  CHECK(failure.type() == type);
  CHECK(fmt::format("{}", failure) == code_number);
  CHECK(fmt::format("{}", failure.type()) == type_number);
}
