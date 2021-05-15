#include "unicode.hpp"
#include <utf8proc.h>

namespace ice::unicode {
namespace {

constexpr ice::error make_unicode_error(utf8proc_ssize_t code) noexcept
{
  switch (code) {
  case 0:
    return ice::errc::success;
  case UTF8PROC_ERROR_NOMEM:
    return ice::errc::unicode_not_enough_memory;
  case UTF8PROC_ERROR_OVERFLOW:
    return ice::errc::unicode_buffer_too_small;
  case UTF8PROC_ERROR_INVALIDUTF8:
    return ice::errc::unicode_invalid_utf8;
  case UTF8PROC_ERROR_NOTASSIGNED:
    return ice::errc::unicode_unassigned;
  case UTF8PROC_ERROR_INVALIDOPTS:
    return ice::errc::unicode_invalid_options;
  }
  return ice::errc::unknown;
}

}  // namespace

bool is_lower(char32_t uc) noexcept
{
  return utf8proc_islower(static_cast<utf8proc_int32_t>(uc));
}

bool is_upper(char32_t uc) noexcept
{
  return utf8proc_isupper(static_cast<utf8proc_int32_t>(uc));
}

char32_t to_lower(char32_t uc) noexcept
{
  return static_cast<char32_t>(utf8proc_tolower(static_cast<utf8proc_int32_t>(uc)));
}

char32_t to_upper(char32_t uc) noexcept
{
  return static_cast<char32_t>(utf8proc_toupper(static_cast<utf8proc_int32_t>(uc)));
}

char32_t to_title(char32_t uc) noexcept
{
  return static_cast<char32_t>(utf8proc_totitle(static_cast<utf8proc_int32_t>(uc)));
}

ice::result<std::string> normalize(std::string_view src, options dec, options enc) noexcept
{
  if (src.empty()) {
    return {};
  }

  const auto src_data = reinterpret_cast<const utf8proc_uint8_t*>(src.data());
  const auto src_size = static_cast<utf8proc_ssize_t>(src.size());

  std::string dst;
  dst.resize(src.size() * sizeof(utf8proc_int32_t));
  const auto dst_data = reinterpret_cast<utf8proc_int32_t*>(dst.data());

  auto dst_size = utf8proc_decompose(src_data, src_size, dst_data, src_size, utf8proc_option_t(dec));
  if (dst_size < 0) {
    return make_unicode_error(dst_size);
  }
  if (dst_size > src_size) {
    return ice::errc::unicode_buffer_too_small;
  }

  dst_size = utf8proc_reencode(dst_data, dst_size, utf8proc_option_t(enc));
  if (dst_size < 0) {
    return make_unicode_error(dst_size);
  }
  dst.resize(static_cast<std::size_t>(dst_size));

  return dst;
}

}  // namespace ice::unicode
