#include "format.hpp"
#include <mutex>

#ifdef _WIN32
#  include <windows.h>
#endif

namespace ice {
namespace {

std::mutex& trace_mutex() noexcept
{
  static std::mutex mutex;
  return mutex;
}

}  // namespace

std::string tr(std::string_view text) noexcept
{
  return std::string{ text };
}

void trace(std::string text, const char* function) noexcept
{
  const auto s = [&]() {
    if (text.empty()) {
      return std::string{ function } + '\n';
    }
    return fmt::format("{}: {}\n", function, text);
  }();
  std::lock_guard lock{ trace_mutex() };
  std::fputs(s.data(), stderr);
  std::fflush(stderr);
#ifdef _WIN32
  if (IsDebuggerPresent()) {
    OutputDebugString(s.data());
  }
#endif
}

}  // namespace ice
