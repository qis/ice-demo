#include "application.hpp"
#include <atomic>
#include <mutex>

#ifdef _WIN32
#  include <windows.h>
#  include <shellapi.h>
#else
#  include <ice/os/xcb/config.hpp>
#endif

#ifdef __linux__
#  include <climits>
#endif

#ifdef __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/types.h>
#endif

namespace ice {

ice::error_info make_error_info(errc) noexcept
{
  constexpr auto text = [](int code) -> std::string {
    return std::generic_category().message(code);
  };
  return { "common", text };
}

}  // namespace ice

namespace ice::system {

ice::error_info make_error_info(errc) noexcept
{
  constexpr auto text = [](int code) -> std::string {
#ifdef _WIN32
    LPSTR buffer = nullptr;
    const DWORD length = FormatMessageA(
      FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
      static_cast<DWORD>(code), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&buffer), 0, nullptr);
    if (buffer && length) {
      std::string text{ buffer };
      LocalFree(buffer);
      return text;
    }
    return ice::error_info::format(code);
#else
    return std::system_category().message(code);
#endif
  };
  return { "system", text };
}

}  // namespace ice::system

namespace std {

ice::error_info make_error_info(std::errc) noexcept
{
  constexpr auto text = [](int code) -> std::string {
    return std::generic_category().message(code);
  };
  return { "generic", text };
}

}  // namespace std

namespace ice::application {
namespace detail {
namespace {

std::atomic<std::mutex*> global_exit_mutex{ nullptr };
char* global_exit_message{ nullptr };

}  // namespace

void message(std::string_view message) noexcept
{
  std::fputs(message.data(), stderr);
  std::fputc('\n', stderr);
  std::fflush(stderr);
#ifdef _WIN32
  MSGBOXPARAMS mbi{};
  mbi.cbSize = sizeof(mbi);
  mbi.hwndOwner = GetActiveWindow();
  mbi.hInstance = nullptr;
  mbi.lpszText = message.data();
  mbi.lpszCaption = "Critical Error";
  mbi.dwStyle = MB_OK | MB_ICONERROR | MB_SETFOREGROUND;
  mbi.lpszIcon = nullptr;
  mbi.dwContextHelpId = 0;
  mbi.lpfnMsgBoxCallback = nullptr;
  mbi.dwLanguageId = LANG_NEUTRAL;
  MessageBoxIndirect(&mbi);
#endif
}

[[noreturn]] void exit(std::string_view message) noexcept
{
  std::mutex* mutex{ nullptr };
  auto mutex_replacement = new std::mutex{};
  if (global_exit_mutex.compare_exchange_strong(mutex, mutex_replacement)) {
    mutex = mutex_replacement;
  } else {
    delete mutex_replacement;
  }
  mutex->lock();
  global_exit_message = new char[message.size() + 1];
  std::memcpy(global_exit_message, message.data(), message.size());
  global_exit_message[message.size()] = '\0';
  std::atexit([]() {
    ice::application::message(global_exit_message ? global_exit_message : "application terminated");
    if (global_exit_message) {
      delete[] global_exit_message;
      global_exit_message = nullptr;
    }
    if (std::mutex* mutex = global_exit_mutex.load(std::memory_order_acquire)) {
      mutex->unlock();
      if (global_exit_mutex.compare_exchange_strong(mutex, nullptr)) {
        delete mutex;
      }
    }
  });
  std::exit(EXIT_FAILURE);
}

}  // namespace detail

namespace {

std::string global_args_data;
std::vector<std::string_view> global_args;

#ifdef _WIN32
std::atomic<HINSTANCE> global_instance{ nullptr };
#endif

std::string th(int i) noexcept
{
  const auto s = std::to_string(i);
  const auto j = i % 10;
  const auto k = i % 100;
  if (j == 1 && k != 11) {
    return s + "st";
  }
  if (j == 2 && k != 12) {
    return s + "nd";
  }
  if (j == 3 && k != 13) {
    return s + "rd";
  }
  return s + "th";
}

}  // namespace

void initialize(int argc, char* argv[], bool load) noexcept
{
  if (load) {
    ice::application::load();
  }
#ifdef _WIN32
  global_instance.store(GetModuleHandle(nullptr), std::memory_order_release);
#endif
  global_args.clear();
  global_args.reserve(static_cast<std::size_t>(argc));
  for (auto i = 1; i < argc; i++) {
    global_args.emplace_back(argv[i]);
  }
}

#ifdef _WIN32

void initialize(HINSTANCE instance, LPSTR cmd, bool load) noexcept
{
  if (load) {
    ice::application::load();
  }
  global_instance.store(instance, std::memory_order_release);
  global_args_data.clear();
  global_args = [&]() -> std::vector<std::string_view> {
    if (!cmd) {
      return {};
    }

    auto cmdlen = static_cast<int>(std::strlen(cmd));
    if (cmdlen < 1) {
      return {};
    }

    auto wcslen = MultiByteToWideChar(CP_UTF8, 0, cmd, cmdlen, nullptr, 0);
    if (wcslen < 1) {
      const auto e = ice::make_error<ice::system::errc>(GetLastError());
      ice::application::exit("Could not get command line size.\n{}", e);
    }

    std::wstring wcs;
    wcs.resize(static_cast<std::size_t>(wcslen) + 2);
    wcslen = MultiByteToWideChar(CP_UTF8, 0, cmd, cmdlen, wcs.data(), wcslen + 1);
    if (wcslen < 1) {
      const auto e = ice::make_error<ice::system::errc>(GetLastError());
      ice::application::exit("Could not read command line.\n{}", e);
    }

    int argc = 0;
    const auto argv = CommandLineToArgvW(wcs.data(), &argc);
    if (!argv) {
      const auto e = ice::make_error<ice::system::errc>(GetLastError());
      ice::application::exit("Could not parse command line.\n{}", e);
    }
    std::unique_ptr<LPWSTR, decltype(&LocalFree)> argv_handle{ argv, LocalFree };

    if (argc < 1) {
      return {};
    }

    struct entry {
      std::size_t offset{ 0 };
      std::size_t size{ 0 };
    };

    std::vector<entry> entries;
    entries.resize(static_cast<std::size_t>(argc));

    for (auto i = 0; i < argc; i++) {
      entries[i].offset = global_args_data.size();

      const auto arglen = static_cast<int>(std::wcslen(argv[i]));
      if (arglen < 1) {
        global_args_data.push_back('\0');
        continue;
      }

      auto estlen = WideCharToMultiByte(CP_UTF8, 0, argv[i], arglen, nullptr, 0, nullptr, nullptr);
      if (estlen < 1) {
        const auto e = ice::make_error<ice::system::errc>(GetLastError());
        ice::application::exit("Could not get {} command line argument.\n{}", th(i), e);
      }

      std::string str;
      str.resize(static_cast<std::size_t>(estlen) + 2);
      estlen = WideCharToMultiByte(CP_UTF8, 0, argv[i], arglen, str.data(), estlen + 1, nullptr, nullptr);
      if (estlen < 0) {
        const auto e = ice::make_error<ice::system::errc>(GetLastError());
        ice::application::exit("Could not read {} command line argument{}.\n{}", th(i), e);
      }

      str[static_cast<std::size_t>(estlen)] = '\0';
      global_args_data.append(str.data(), static_cast<std::size_t>(estlen) + 1);

      entries[i].size = static_cast<std::size_t>(estlen);
    }

    std::vector<std::string_view> args;
    args.reserve(entries.size());
    for (const auto& entry : entries) {
      args.emplace_back(global_args_data.data() + entry.offset, entry.size);
    }
    return args;
  }();
}

HINSTANCE instance() noexcept
{
  if (auto instance = global_instance.load(std::memory_order_acquire)) {
    return instance;
  }
  return GetModuleHandle(nullptr);
}

#endif

void load() noexcept
{
  load<std::errc>();
  load<ice::errc>();
  load<ice::system::errc>();
#ifndef _WIN32
  load<ice::os::xcb::errc>();
#endif
}

void unload() noexcept
{
  unload<std::errc>();
  unload<ice::errc>();
  unload<ice::system::errc>();
#ifndef _WIN32
  unload<ice::os::xcb::errc>();
#endif
}

std::span<const std::string_view> args() noexcept
{
  return global_args;
}

// http://www.tech.theplayhub.com/finding_current_executables_path_without_procselfexe-7
// =====================================================================================
// Mac OS X: _NSGetExecutablePath() (man 3 dyld)
// Linux: readlink /proc/self/exe
// Solaris: getexecname()
// FreeBSD: sysctl CTL_KERN KERN_PROC KERN_PROC_PATHNAME -1
// FreeBSD if it has procfs: readlink /proc/curproc/file (FreeBSD doesn't have procfs by default)
// NetBSD: readlink /proc/curproc/exe
// DragonFly BSD: readlink /proc/curproc/file
// Windows: GetModuleFileName() with hModule = NULL

std::filesystem::path executable() noexcept
{
  std::string str;
#ifdef _WIN32
  DWORD size = 0;
  do {
    str.resize(str.size() + MAX_PATH);
    size = GetModuleFileNameA(nullptr, &str[0], static_cast<DWORD>(str.size()));
  } while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);
  str.resize(size);
#endif
#ifdef __linux__
  if (const auto s = realpath("/proc/self/exe", nullptr)) {
    str = s;
    free(s);  // NOLINT(cppcoreguidelines-no-malloc)
  }
#endif
#ifdef __FreeBSD__
  int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
  size_t size = 0;
  if (sysctl(mib, 4, nullptr, &size, nullptr, 0) == 0) {
    str.resize(size);
    if (sysctl(mib, 4, &str[0], &size, nullptr, 0) == 0) {
      str.resize(size > 0 ? size - 1 : 0);
    } else {
      str.clear();
    }
  }
#endif
  std::error_code ec;
  auto path = ice::make_path(str);
  path = std::filesystem::canonical(path, ec);
  if (ec) {
    return str;
  }
  return path;
}

}  // namespace ice::application
