#include "error_code.hpp"
#include <ice/format.hpp>
#include <algorithm>
#include <iterator>
#include <mutex>
#include <set>

namespace ice {
namespace {

struct error_name_entry {
  ice::error_type type{};
  const char* name = nullptr;

  constexpr bool operator<(error_name_entry other) const noexcept
  {
    return type < other.type;
  }

  constexpr bool operator==(ice::error_type other) const noexcept
  {
    return type == other;
  }
};

struct error_text_entry {
  ice::error_type type{};
  std::string (*text)(int code) = nullptr;

  constexpr bool operator<(error_text_entry other) const noexcept
  {
    return type < other.type;
  }

  constexpr bool operator==(ice::error_type other) const noexcept
  {
    return type == other;
  }
};

std::set<error_name_entry>& error_name_data() noexcept
{
  static std::set<error_name_entry> error_name_set;
  return error_name_set;
};

std::set<error_text_entry>& error_text_data() noexcept
{
  static std::set<error_text_entry> error_text_set;
  return error_text_set;
};

std::mutex& error_mutex() noexcept
{
  static std::mutex error_mutex;
  return error_mutex;
}

}  // namespace

void error_info::set(ice::error_type type, const char* name) noexcept
{
  std::lock_guard lock{ error_mutex() };
  auto& data = error_name_data();
  if (name) {
    data.insert({ type, name });
  } else if (const auto it = std::find(data.begin(), data.end(), type); it != data.end()) {
    data.erase(it);
  }
}

void error_info::set(ice::error_type type, std::string (*text)(int code)) noexcept
{
  std::lock_guard lock{ error_mutex() };
  auto& data = error_text_data();
  if (text) {
    data.insert({ type, text });
  } else if (const auto it = std::find(data.begin(), data.end(), type); it != data.end()) {
    data.erase(it);
  }
}

const char* error_info::get(ice::error_type type) noexcept
{
  std::lock_guard lock{ error_mutex() };
  const auto& data = error_name_data();
  if (const auto it = std::find(data.begin(), data.end(), type); it != data.end()) {
    return it->name;
  }
  return nullptr;
}

std::string error_info::get(ice::error_type type, int code) noexcept
{
  std::lock_guard lock{ error_mutex() };
  const auto& data = error_text_data();
  if (const auto it = std::find(data.begin(), data.end(), type); it != data.end()) {
    return it->text(code);
  }
  return fmt::format("{:08X}", static_cast<unsigned>(code));
}

std::string error_info::format(int code) noexcept
{
  return fmt::format("error code {:08X}", static_cast<unsigned>(code));
}

}  // namespace ice
