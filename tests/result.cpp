#include "result.hpp"
#include "concepts.hpp"
#include <type_traits>

static_assert(std::is_default_constructible_v<ice::result<void>>);
static_assert(std::is_move_constructible_v<ice::result<void>>);
static_assert(std::is_copy_constructible_v<ice::result<void>>);
static_assert(std::is_move_assignable_v<ice::result<void>>);
static_assert(std::is_copy_assignable_v<ice::result<void>>);

static_assert(std::is_default_constructible_v<flexible_type>);
static_assert(std::is_default_constructible_v<ice::result<flexible_type>>);
static_assert(std::is_move_constructible_v<flexible_type>);
static_assert(std::is_move_constructible_v<ice::result<flexible_type>>);
static_assert(std::is_copy_constructible_v<flexible_type>);
static_assert(std::is_copy_constructible_v<ice::result<flexible_type>>);
static_assert(std::is_move_assignable_v<flexible_type>);
static_assert(std::is_move_assignable_v<ice::result<flexible_type>>);
static_assert(std::is_copy_assignable_v<flexible_type>);
static_assert(std::is_copy_assignable_v<ice::result<flexible_type>>);

static_assert(!std::is_default_constructible_v<no_default_constructible_type>);
static_assert(!std::is_default_constructible_v<ice::result<no_default_constructible_type>>);
static_assert(std::is_move_constructible_v<no_default_constructible_type>);
static_assert(std::is_move_constructible_v<ice::result<no_default_constructible_type>>);
static_assert(std::is_copy_constructible_v<no_default_constructible_type>);
static_assert(std::is_copy_constructible_v<ice::result<no_default_constructible_type>>);
static_assert(std::is_move_assignable_v<no_default_constructible_type>);
static_assert(std::is_move_assignable_v<ice::result<no_default_constructible_type>>);
static_assert(std::is_copy_assignable_v<no_default_constructible_type>);
static_assert(std::is_copy_assignable_v<ice::result<no_default_constructible_type>>);

static_assert(std::is_default_constructible_v<no_move_constructible_type>);
static_assert(std::is_default_constructible_v<ice::result<no_move_constructible_type>>);
static_assert(!std::is_move_constructible_v<no_move_constructible_type>);
static_assert(!std::is_move_constructible_v<ice::result<no_move_constructible_type>>);
static_assert(std::is_copy_constructible_v<no_move_constructible_type>);
static_assert(std::is_copy_constructible_v<ice::result<no_move_constructible_type>>);
static_assert(std::is_move_assignable_v<no_move_constructible_type>);
static_assert(std::is_move_assignable_v<ice::result<no_move_constructible_type>>);
static_assert(std::is_copy_assignable_v<no_move_constructible_type>);
static_assert(std::is_copy_assignable_v<ice::result<no_move_constructible_type>>);

static_assert(std::is_default_constructible_v<no_copy_constructible_type>);
static_assert(std::is_default_constructible_v<ice::result<no_copy_constructible_type>>);
static_assert(std::is_move_constructible_v<no_copy_constructible_type>);
static_assert(std::is_move_constructible_v<ice::result<no_copy_constructible_type>>);
static_assert(!std::is_copy_constructible_v<no_copy_constructible_type>);
static_assert(!std::is_copy_constructible_v<ice::result<no_copy_constructible_type>>);
static_assert(std::is_move_assignable_v<no_copy_constructible_type>);
static_assert(std::is_move_assignable_v<ice::result<no_copy_constructible_type>>);
static_assert(std::is_copy_assignable_v<no_copy_constructible_type>);
static_assert(std::is_copy_assignable_v<ice::result<no_copy_constructible_type>>);

static_assert(std::is_default_constructible_v<no_move_assignable_type>);
static_assert(std::is_default_constructible_v<ice::result<no_move_assignable_type>>);
static_assert(std::is_move_constructible_v<no_move_assignable_type>);
static_assert(std::is_move_constructible_v<ice::result<no_move_assignable_type>>);
static_assert(std::is_copy_constructible_v<no_move_assignable_type>);
static_assert(std::is_copy_constructible_v<ice::result<no_move_assignable_type>>);
static_assert(!std::is_move_assignable_v<no_move_assignable_type>);
static_assert(!std::is_move_assignable_v<ice::result<no_move_assignable_type>>);
static_assert(std::is_copy_assignable_v<no_move_assignable_type>);
static_assert(std::is_copy_assignable_v<ice::result<no_move_assignable_type>>);

static_assert(std::is_default_constructible_v<no_copy_assignable_type>);
static_assert(std::is_default_constructible_v<ice::result<no_copy_assignable_type>>);
static_assert(std::is_move_constructible_v<no_copy_assignable_type>);
static_assert(std::is_move_constructible_v<ice::result<no_copy_assignable_type>>);
static_assert(std::is_copy_constructible_v<no_copy_assignable_type>);
static_assert(std::is_copy_constructible_v<ice::result<no_copy_assignable_type>>);
static_assert(std::is_move_assignable_v<no_copy_assignable_type>);
static_assert(std::is_move_assignable_v<ice::result<no_copy_assignable_type>>);
static_assert(!std::is_copy_assignable_v<no_copy_assignable_type>);
static_assert(!std::is_copy_assignable_v<ice::result<no_copy_assignable_type>>);

#include <doctest/doctest.h>

TEST_CASE("ice::result<void>")
{
  ice::application application;

  constexpr ice::result<void> success;
  static_assert(success.type() == ice::error_type::success);
  static_assert(success.code() == 0);
  static_assert(success.error() == ice::error{});
  static_assert(success);

  static_assert(success != static_cast<std::errc>(0));
  static_assert(static_cast<std::errc>(0) != success);

  static_assert(success != std::errc::no_such_file_or_directory);
  static_assert(std::errc::no_such_file_or_directory != success);

  static_assert(success != ice::make_error_code(std::errc::no_such_file_or_directory));
  static_assert(ice::make_error_code(std::errc::no_such_file_or_directory) != success);

  static_assert(success != ice::make_error(std::errc::no_such_file_or_directory));
  static_assert(ice::make_error(std::errc::no_such_file_or_directory) != success);

  static_assert(success == ice::error{});
  static_assert(ice::error{} == success);

  constexpr ice::result<void> failure{ std::errc::no_such_file_or_directory };
  static_assert(failure.type() == ice::make_error_type<std::errc>());
  static_assert(failure.code() == static_cast<int>(std::errc::no_such_file_or_directory));
  static_assert(failure.error() == std::errc::no_such_file_or_directory);
  static_assert(!failure);

  static_assert(failure != static_cast<std::errc>(0));
  static_assert(static_cast<std::errc>(0) != failure);

  static_assert(failure != std::errc::file_exists);
  static_assert(std::errc::file_exists != failure);

  static_assert(failure == std::errc::no_such_file_or_directory);
  static_assert(std::errc::no_such_file_or_directory == failure);

  static_assert(failure != ice::make_error_code(std::errc::file_exists));
  static_assert(ice::make_error_code(std::errc::file_exists) != failure);

  static_assert(failure == ice::make_error_code(std::errc::no_such_file_or_directory));
  static_assert(ice::make_error_code(std::errc::no_such_file_or_directory) == failure);

  static_assert(failure != ice::make_error(std::errc::file_exists));
  static_assert(ice::make_error(std::errc::file_exists) != failure);

  static_assert(failure == ice::make_error(std::errc::no_such_file_or_directory));
  static_assert(ice::make_error(std::errc::no_such_file_or_directory) == failure);

  static_assert(failure != ice::error{});
  static_assert(ice::error{} != failure);
}

TEST_CASE("ice::result<std::string>")
{
  ice::application application;

  ice::result<std::string> success{ "ok" };
  CHECK(success.type() == ice::error_type::success);
  CHECK(success.code() == 0);
  CHECK(success.error() == ice::error{});
  CHECK(success.value() == "ok");
  CHECK(success);

  CHECK(success != std::errc::no_such_file_or_directory);
  CHECK(std::errc::no_such_file_or_directory != success);

  CHECK(success != ice::make_error_code(std::errc::no_such_file_or_directory));
  CHECK(ice::make_error_code(std::errc::no_such_file_or_directory) != success);

  CHECK(success != ice::make_error(std::errc::no_such_file_or_directory));
  CHECK(ice::make_error(std::errc::no_such_file_or_directory) != success);

  CHECK(success == ice::error{});
  CHECK(ice::error{} == success);

  ice::result<std::string> failure{ std::errc::no_such_file_or_directory };
  CHECK(failure.type() == ice::make_error_type<std::errc>());
  CHECK(failure.code() == static_cast<int>(std::errc::no_such_file_or_directory));
  CHECK(failure.error() == std::errc::no_such_file_or_directory);
  CHECK(!failure);

  CHECK(failure != static_cast<std::errc>(0));
  CHECK(static_cast<std::errc>(0) != failure);

  CHECK(failure != std::errc::file_exists);
  CHECK(std::errc::file_exists != failure);

  CHECK(failure == std::errc::no_such_file_or_directory);
  CHECK(std::errc::no_such_file_or_directory == failure);

  CHECK(failure != ice::make_error_code(std::errc::file_exists));
  CHECK(ice::make_error_code(std::errc::file_exists) != failure);

  CHECK(failure == ice::make_error_code(std::errc::no_such_file_or_directory));
  CHECK(ice::make_error_code(std::errc::no_such_file_or_directory) == failure);

  CHECK(failure != ice::make_error(std::errc::file_exists));
  CHECK(ice::make_error(std::errc::file_exists) != failure);

  CHECK(failure == ice::make_error(std::errc::no_such_file_or_directory));
  CHECK(ice::make_error(std::errc::no_such_file_or_directory) == failure);

  CHECK(failure != ice::error{});
  CHECK(ice::error{} != failure);
}

constexpr ice::result<void> result_return_void() noexcept
{
  return {};
}

constexpr ice::result<void> result_return_void_error_enum() noexcept
{
  return std::errc::no_such_file_or_directory;
}

constexpr ice::result<void> result_return_void_error_code() noexcept
{
  return ice::make_error_code(std::errc::no_such_file_or_directory);
}

constexpr ice::result<void> result_return_void_error() noexcept
{
  return ice::make_error(std::errc::no_such_file_or_directory);
}

ice::result<void> result_co_return_void() noexcept
{
  co_return {};
}

ice::result<void> result_co_return_void_error_enum() noexcept
{
  co_return std::errc::no_such_file_or_directory;
}

ice::result<void> result_co_return_void_error_code() noexcept
{
  co_return ice::make_error_code(std::errc::no_such_file_or_directory);
}

ice::result<void> result_co_return_void_error() noexcept
{
  co_return ice::make_error(std::errc::no_such_file_or_directory);
}

TEST_CASE("ice::result<void> syntax")
{
  ice::application application;

  static_assert(result_return_void());
  static_assert(result_return_void_error_enum() == std::errc::no_such_file_or_directory);
  static_assert(result_return_void_error_code() == std::errc::no_such_file_or_directory);
  static_assert(result_return_void_error() == std::errc::no_such_file_or_directory);

  CHECK(result_co_return_void());
  CHECK(result_co_return_void_error_enum() == std::errc::no_such_file_or_directory);
  CHECK(result_co_return_void_error_code() == std::errc::no_such_file_or_directory);
  CHECK(result_co_return_void_error() == std::errc::no_such_file_or_directory);
}

#include <string_view>

constexpr ice::result<std::string_view> result_return_string_view() noexcept
{
  return {};
}

constexpr ice::result<std::string_view> result_return_string_view_value() noexcept
{
  std::string_view sv = "onetwo";
  return sv;
}

constexpr ice::result<std::string_view> result_return_string_view_initializer_list() noexcept
{
  return { "onetwo", 3 };
}

constexpr ice::result<std::string_view> result_return_string_view_error_enum() noexcept
{
  return std::errc::no_such_file_or_directory;
}

constexpr ice::result<std::string_view> result_return_string_view_error_code() noexcept
{
  return ice::make_error_code(std::errc::no_such_file_or_directory);
}

constexpr ice::result<std::string_view> result_return_string_view_error() noexcept
{
  return ice::make_error(std::errc::no_such_file_or_directory);
}

ice::result<std::string_view> result_co_return_string_view() noexcept
{
  co_return {};
}

ice::result<std::string_view> result_co_return_string_view_value() noexcept
{
  std::string_view sv = "onetwo";
  co_return sv;
}

ice::result<std::string_view> result_co_return_string_view_initializer_list() noexcept
{
  co_return { "onetwo", 3 };
}

ice::result<std::string_view> result_co_return_string_view_error_enum() noexcept
{
  co_return std::errc::no_such_file_or_directory;
}

ice::result<std::string_view> result_co_return_string_view_error_code() noexcept
{
  co_return ice::make_error_code(std::errc::no_such_file_or_directory);
}

ice::result<std::string_view> result_co_return_string_view_error() noexcept
{
  co_return ice::make_error(std::errc::no_such_file_or_directory);
}

TEST_CASE("result syntax with std::string_view")
{
  ice::application application;

  CHECK(result_return_string_view());
  CHECK(result_return_string_view()->empty());
  CHECK(result_return_string_view_value());
  CHECK(result_return_string_view_value().value() == "onetwo");
  CHECK(result_return_string_view_initializer_list());
  CHECK(result_return_string_view_initializer_list().value() == "one");
  static_assert(result_return_string_view_error_enum() == std::errc::no_such_file_or_directory);
  static_assert(result_return_string_view_error_code() == std::errc::no_such_file_or_directory);
  static_assert(result_return_string_view_error() == std::errc::no_such_file_or_directory);

  CHECK(result_co_return_string_view());
  CHECK(result_co_return_string_view().value().empty());
  CHECK(result_co_return_string_view_value());
  CHECK(result_co_return_string_view_value().value() == "onetwo");
  CHECK(result_co_return_string_view_initializer_list());
  CHECK(result_co_return_string_view_initializer_list().value() == "one");
  CHECK(result_co_return_string_view_error_enum() == std::errc::no_such_file_or_directory);
  CHECK(result_co_return_string_view_error_code() == std::errc::no_such_file_or_directory);
  CHECK(result_co_return_string_view_error() == std::errc::no_such_file_or_directory);
}

ice::result<no_default_constructible_type> result_return_no_default_constructible() noexcept
{
  return 3;
}

ice::result<no_default_constructible_type> result_return_no_default_constructible_error() noexcept
{
  return ice::make_error(std::errc::no_such_file_or_directory);
}

ice::result<restricted_type> result_return_restricted_type() noexcept
{
  return 3;
}

ice::result<restricted_type> result_return_restricted_type_error() noexcept
{
  return ice::make_error(std::errc::no_such_file_or_directory);
}

ice::result<no_default_constructible_type> result_co_return_no_default_constructible() noexcept
{
  co_return 3;
}

ice::result<no_default_constructible_type> result_co_return_no_default_constructible_error() noexcept
{
  co_return ice::make_error(std::errc::no_such_file_or_directory);
}

ice::result<restricted_type> result_co_return_restricted_type() noexcept
{
  co_return 3;
}

ice::result<restricted_type> result_co_return_restricted_type_error() noexcept
{
  co_return ice::make_error(std::errc::no_such_file_or_directory);
}

TEST_CASE("result construction")
{
  ice::application application;

  CHECK(result_return_no_default_constructible());
  CHECK(result_return_no_default_constructible()->value == 3);
  CHECK(!result_return_no_default_constructible_error());
  CHECK(result_return_no_default_constructible_error() == std::errc::no_such_file_or_directory);

  CHECK(result_return_restricted_type());
  CHECK(result_return_restricted_type()->value == 3);
  CHECK(!result_return_restricted_type_error());
  CHECK(result_return_restricted_type_error() == std::errc::no_such_file_or_directory);

  CHECK(result_co_return_no_default_constructible());
  CHECK(result_co_return_no_default_constructible()->value == 3);
  CHECK(!result_co_return_no_default_constructible_error());
  CHECK(result_co_return_no_default_constructible_error() == std::errc::no_such_file_or_directory);

  CHECK(result_co_return_restricted_type());
  CHECK(result_co_return_restricted_type()->value == 3);
  CHECK(!result_co_return_restricted_type_error());
  CHECK(result_co_return_restricted_type_error() == std::errc::no_such_file_or_directory);
}

struct task {
  struct promise_type : ice::promise_base {
    static constexpr auto get_return_object() noexcept
    {
      return task{};
    }

    static constexpr auto initial_suspend() noexcept
    {
      return ice::suspend_never{};
    }

    static constexpr auto final_suspend() noexcept
    {
      return ice::suspend_never{};
    }

    static constexpr void return_void() noexcept
    {}

    static void return_error(ice::error e) noexcept
    {
      error = e;
    }
  };

  static inline ice::error error;
};

ice::result<void> inner()
{
  co_return std::errc::interrupted;
}

ice::result<void> outer()
{
  co_await inner();
  co_return std::errc::no_such_file_or_directory;
}

TEST_CASE("result awaitable")
{
  ice::application application;

  task::error = {};
  auto never_set = false;
  [&]() -> task {
    co_await outer();
    never_set = true;
  }();

  CHECK(!never_set);
  CHECK(task::error == std::errc::interrupted);
  CHECK(fmt::format("{}", task::error.type()) == "system");
  CHECK(fmt::format("{}", task::error) == "interrupted");
}
