#pragma once
#include <ice/format.hpp>
#include <ice/path.hpp>
#include <span>
#include <cstdlib>

#ifdef _WIN32
#  include <windows.h>
#endif

namespace ice::application {
namespace detail {

ICE_API void message(std::string_view message) noexcept;
[[noreturn]] ICE_API void exit(std::string_view message) noexcept;

}  // namespace detail

ICE_API void initialize(int argc, char* argv[], bool load = true) noexcept;

#ifdef _WIN32
ICE_API void initialize(HINSTANCE instance, LPSTR cmd, bool load = true) noexcept;
ICE_API HINSTANCE instance() noexcept;
#endif

ICE_API void load() noexcept;

template <ErrorCodeType T>
constexpr void load() noexcept
{
  const auto info = make_error_info(T{});
  ice::error_info::set(ice::make_error_type<T>(), info.name);
  ice::error_info::set(ice::make_error_type<T>(), info.text);
}

ICE_API void unload() noexcept;

template <ErrorCodeType T>
constexpr void unload() noexcept
{
  ice::error_info::set(ice::make_error_type<T>(), static_cast<decltype(ice::error_info::name)>(nullptr));
  ice::error_info::set(ice::make_error_type<T>(), static_cast<decltype(ice::error_info::text)>(nullptr));
}

ICE_API std::span<const std::string_view> args() noexcept;
ICE_API std::filesystem::path executable() noexcept;

template <typename... Args>
constexpr void message(std::string_view text, Args&&... args) noexcept
{
  detail::message(fmt::format(ice::tr(text), std::forward<Args>(args)...));
}

template <typename... Args>
[[noreturn]] constexpr void exit(std::string_view text, Args&&... args) noexcept
{
  detail::exit(fmt::format(ice::tr(text), std::forward<Args>(args)...));
}

}  // namespace ice::application
