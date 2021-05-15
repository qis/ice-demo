#pragma once
#include <ice/config.hpp>
#include <compare>
#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>

namespace ice {
namespace detail {

constexpr unsigned error_hash(const char* s) noexcept
{
  unsigned hash = 0x811C9DC5;
  while (*s) {
    hash = (static_cast<unsigned>(*s++) ^ hash) * 0x01000193;
  }
  return hash;
}

template <typename T>
constexpr unsigned error_hash() noexcept
{
  return error_hash(ICE_FUNCTION);
}

}  // namespace detail

// ================================================================================================
// underlying error code type
// ================================================================================================

template <typename T, typename V = std::remove_cvref_t<T>>
concept UnderlyingErrorCodeType = requires(T)
{
  requires(std::is_integral_v<V> && !std::is_same_v<std::remove_cvref_t<V>, bool>);
  requires(sizeof(V) <= 4);
};

// ================================================================================================
// error code type
// ================================================================================================

template <typename T, typename V = std::remove_cvref_t<T>>
concept ErrorCodeType = requires(T)
{
  requires(std::is_enum_v<V> && !std::is_convertible_v<V, std::underlying_type_t<V>>);
  requires(UnderlyingErrorCodeType<std::underlying_type_t<V>>);
};

// ================================================================================================
// error type
// ================================================================================================

enum class error_type : unsigned {
  success = 0,
  unknown = 0xFFFFFFFF,
};

template <ErrorCodeType T>
constexpr ice::error_type make_error_type() noexcept
{
  constexpr auto type = detail::error_hash<std::remove_cvref_t<T>>();
  static_assert(type != static_cast<unsigned>(ice::error_type::success));
  static_assert(type != static_cast<unsigned>(ice::error_type::unknown));
  return static_cast<ice::error_type>(type);
}

// ================================================================================================
// error code
// ================================================================================================

template <ErrorCodeType T>
class error_code {
public:
  error_code() noexcept = default;
  error_code(error_code&& other) noexcept = default;
  error_code(const error_code& other) noexcept = default;
  error_code& operator=(error_code&& other) noexcept = default;
  error_code& operator=(const error_code& other) noexcept = default;

  constexpr error_code(T code) noexcept
    : code_(static_cast<int>(code))
  {}

  explicit constexpr operator bool() const noexcept
  {
    return code_ != 0;
  }

  explicit constexpr operator T() const noexcept
  {
    return static_cast<T>(code_);
  }

  constexpr int code() const noexcept
  {
    return code_;
  }

  constexpr ice::error_type type() const noexcept
  {
    return ice::make_error_type<T>();
  }

  friend constexpr auto operator<=>(error_code, error_code) noexcept = default;

private:
  int code_{ 0 };
};

template <ErrorCodeType T, ErrorCodeType U>
constexpr bool operator==(error_code<T> lhs, error_code<U> rhs) noexcept
{
  if constexpr (lhs.type() == rhs.type()) {
    return lhs.code() == rhs.code();
  }
  return false;
}

template <typename T>
struct is_error_code : std::false_type {};

template <typename T>
struct is_error_code<error_code<T>> : std::true_type {};

template <typename T>
constexpr bool is_error_code_v = is_error_code<T>::value;

template <typename T>
concept ErrorCode = is_error_code_v<std::remove_cvref_t<T>>;

template <ErrorCodeType T>
constexpr ice::error_code<T> make_error_code(T code) noexcept
{
  return { code };
}

template <ErrorCodeType T, UnderlyingErrorCodeType U>
constexpr ice::error_code<T> make_error_code(U code) noexcept
{
  return { static_cast<T>(code) };
}

// ================================================================================================
// error info
// ================================================================================================

struct error_info {
  const char* name{ nullptr };
  std::string (*text)(int code){ nullptr };

  ICE_API static void set(ice::error_type type, const char* name) noexcept;
  ICE_API static void set(ice::error_type type, std::string (*text)(int code)) noexcept;

  ICE_API static const char* get(ice::error_type type) noexcept;
  ICE_API static std::string get(ice::error_type type, int code) noexcept;

  ICE_API static std::string format(int code) noexcept;
};

ICE_API ice::error_info make_error_info(ice::errc) noexcept;

}  // namespace ice

namespace std {

ICE_API ice::error_info make_error_info(std::errc) noexcept;

}  // namespace std
