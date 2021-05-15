#pragma once
#include <ice/config.hpp>
#include <filesystem>
#include <string>
#include <string_view>

namespace ice {

inline std::filesystem::path make_path(std::string_view path) noexcept
{
  return { std::u8string_view{ reinterpret_cast<const char8_t*>(path.data()), path.size() } };
}

}  // namespace ice
