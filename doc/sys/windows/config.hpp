#pragma once
#include <ice/error.hpp>
#include <windows.h>
#include <shellapi.h>

namespace ice::sys::windows {

// ================================================================================================
// error
// ================================================================================================

enum class errc : DWORD;

constexpr ice::error_code<errc> make_error(DWORD code) noexcept
{
  return static_cast<errc>(code);
}

inline ice::error_code<errc> get_last_error() noexcept
{
  return make_error(GetLastError());
}

inline ice::error_info make_error_info(errc) noexcept
{
  constexpr auto text = [](int code) -> std::string {
    LPSTR buffer = nullptr;
    // clang-format off
    const DWORD length = FormatMessageA(
      FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
      static_cast<DWORD>(code), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&buffer), 0, nullptr);
    // clang-format on
    if (buffer && length) {
      std::string text{ buffer };
      LocalFree(buffer);
      return text;
    }
    return ice::error_info::format(code);
  };
  return { "windows error", text };
}

}  // namespace ice::sys::windows
