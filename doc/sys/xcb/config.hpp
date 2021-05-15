#pragma once
#include <ice/error.hpp>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

namespace ice::sys::xcb {

// ================================================================================================
// error
// ================================================================================================

enum class errc : int;

inline ice::error_info make_error_info(errc) noexcept
{
  constexpr auto text = [](int code) -> std::string {
    switch (code) {
    case XCB_CONN_ERROR:
      return "stream error";
    case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
      return "extension not supported";
    case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
      return ice::error_info::get(ice::make_error_type<std::errc>(), ENOMEM);
    case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
      return "request length exceeded";
    case XCB_CONN_CLOSED_PARSE_ERR:
      return "display string parse error";
    case XCB_CONN_CLOSED_INVALID_SCREEN:
      return "invalid screen";
    case XCB_CONN_CLOSED_FDPASSING_FAILED:
      return "file descriptor passing operation failed";
    }
    return ice::error_info::get(ice::make_error_type<std::errc>(), code);
  };
  return { "xcb error", text };
}

}  // namespace ice::sys::xcb
