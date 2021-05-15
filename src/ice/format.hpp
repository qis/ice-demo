#pragma once
#include <ice/result.hpp>
#include <fmt/format.h>
#include <filesystem>
#include <string>
#include <string_view>

namespace ice {

ICE_API std::string tr(std::string_view text) noexcept;
ICE_API void trace(std::string text, const char* function) noexcept;

}  // namespace ice

#define ICE_TRACE_FORMAT(...) ice::trace(fmt::format(__VA_ARGS__), ICE_FUNCTION)
#define ICE_TRACE_FUNCTION ice::trace({}, ICE_FUNCTION)

template <>
struct fmt::formatter<ice::error_type> : fmt::formatter<string_view> {
  template <typename FormatContext>
  auto format(const ice::error_type& type, FormatContext& context) noexcept
  {
    if (const auto name = ice::error_info::get(type)) {
      return fmt::formatter<string_view>::format(name, context);
    }
    fmt::basic_memory_buffer<char, 16> name;
    fmt::format_to(name, "{:08X}", static_cast<unsigned>(type));
    return fmt::formatter<string_view>::format({ name.data(), name.size() }, context);
  }
};

template <ice::ErrorCodeType T>
struct fmt::formatter<ice::error_code<T>> : fmt::formatter<string_view> {
  template <typename FormatContext>
  auto format(const ice::error_code<T>& ec, FormatContext& context) noexcept
  {
    const auto text = ice::error_info::get(ec.type(), ec.code());
    return fmt::formatter<string_view>::format({ text.data(), text.size() }, context);
  }
};

template <ice::ErrorCodeType T>
struct fmt::formatter<T> : fmt::formatter<ice::error_code<T>> {};

template <>
struct fmt::formatter<ice::error> : fmt::formatter<string_view> {
  template <typename FormatContext>
  auto format(const ice::error& e, FormatContext& context) noexcept
  {
    const auto text = ice::error_info::get(e.type(), e.code());
    return fmt::formatter<string_view>::format({ text.data(), text.size() }, context);
  }
};

template <>
struct fmt::formatter<std::filesystem::path> : fmt::formatter<string_view> {
  template <typename FormatContext>
  auto format(const std::filesystem::path& path, FormatContext& context) noexcept
  {
    const auto text = path.generic_string();
    return fmt::formatter<string_view>::format({ text.data(), text.size() }, context);
  }
};
