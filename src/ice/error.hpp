#pragma once
#include <ice/error_code.hpp>
#include <type_traits>

namespace ice {

// ================================================================================================
// error
// ================================================================================================

class error {
public:
  template <ErrorCodeType E>
  constexpr error(E ev) noexcept
    : type_(ice::make_error_type<E>())
    , code_(static_cast<int>(ev))
  {}

  template <ErrorCodeType E>
  constexpr error(ice::error_code<E> ec) noexcept
    : type_(ice::make_error_type<E>())
    , code_(ec.code())
  {}

  constexpr error(ice::error_type type, int code) noexcept
    : type_(type)
    , code_(code)
  {}

  error(std::error_code ec) noexcept
    : type_(ice::error_type::unknown)
    , code_(ec.value())
  {
    if (ec.category() == std::generic_category()) {
      type_ = ice::make_error_type<std::errc>();
    } else if (ec.category() == std::system_category()) {
      type_ = ice::make_error_type<ice::system::errc>();
    }
  }

  error() noexcept = default;
  error(error&& other) noexcept = default;
  error(const error& other) noexcept = default;
  error& operator=(error&& other) noexcept = default;
  error& operator=(const error& other) noexcept = default;

  explicit constexpr operator bool() const noexcept
  {
    return code_ != 0;
  }

  constexpr int code() const noexcept
  {
    return code_;
  }

  constexpr ice::error_type type() const noexcept
  {
    return type_;
  }

  friend auto operator<=>(error, error) noexcept = default;


private:
  ice::error_type type_{ 0 };
  int code_{ 0 };
};

template <typename T>
struct is_error : std::false_type {};

template <>
struct is_error<error> : std::true_type {};

template <typename T>
constexpr bool is_error_v = is_error<T>::value;

template <typename T>
concept Error = is_error_v<std::remove_cvref_t<T>>;

constexpr ice::error make_error(ice::error_type type, int code) noexcept
{
  return { type, code };
}

template <ErrorCodeType T, UnderlyingErrorCodeType U>
constexpr ice::error make_error(U code) noexcept
{
  return { static_cast<T>(code) };
}

template <ErrorCodeType T>
constexpr ice::error make_error(T ev) noexcept
{
  return { ev };
}

template <ErrorCodeType T>
constexpr ice::error make_error(ice::error_code<T> ec) noexcept
{
  return { ec };
}

inline ice::error make_error(std::error_code ec) noexcept
{
  return { ec };
}

}  // namespace ice
