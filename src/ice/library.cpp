#include "library.hpp"

#ifdef _WIN32
#  include <windows.h>
#else
#  include <dlfcn.h>
#endif

namespace ice {
namespace {

std::filesystem::path make_shared_library_filename(const std::string& name) noexcept
{
#ifdef _MSC_VER
  std::string filename = name + ".dll";
#else
  std::string filename = "lib" + name + ".so";
#endif
  return ice::make_path(filename);
}

}  // namespace

library::library(library&& other) noexcept
  : handle_(other.handle_)
{
  other.handle_ = nullptr;
}

library& library::operator=(library&& other) noexcept
{
  close();
  handle_ = other.handle_;
  other.handle_ = nullptr;
  return *this;
}

library::library(const std::string& name) noexcept
{
  std::error_code ec;
  path_ = make_shared_library_filename(name);
}

library::library(const std::filesystem::path& path, const std::string& name) noexcept
{
  std::error_code ec;
  const auto filename = make_shared_library_filename(name);
  path_ = std::filesystem::canonical(path / filename, ec);
  if (ec) {
    path_ = path / filename;
  }
}

library::~library()
{
  close();
}

ice::error library::open() noexcept
{
  close();
#ifdef _WIN32
  const auto filename = path_.u8string();
  handle_ = LoadLibraryA(reinterpret_cast<const char*>(filename.data()));
  if (!handle_) {
    return ice::make_error<ice::system::errc>(GetLastError());
  }
#else
  const auto filename = path_.generic_string();
  handle_ = dlopen(filename.data(), RTLD_LAZY);
  if (!handle_) {
    return ice::make_error<std::errc>(errno);
  }
#endif
  return {};
}

ice::error library::close() noexcept
{
  const auto handle = handle_;
  handle_ = nullptr;
  if (handle) {
#ifdef _WIN32
    if (!FreeLibrary(reinterpret_cast<HMODULE>(handle))) {
      return ice::make_error<ice::system::errc>(GetLastError());
    }
#else
    if (const auto code = dlclose(handle)) {
      return ice::make_error<std::errc>(code);
    }
#endif
  }
  return {};
}

void* library::get(const char* symbol) noexcept
{
  if (!handle_) {
    return nullptr;
  }
#ifdef _WIN32
  return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(handle_), symbol));
#else
  return dlsym(handle_, symbol);
#endif
}

}  // namespace ice
