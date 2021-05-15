#pragma once
#include <ice/result.hpp>
#include <array>
#include <string_view>
#include <system_error>
#include <cstdlib>

#ifdef _MSC_VER
#  define ICE_BENCHMARKS_TOOLCHAIN "MSVC"
#else
#  define ICE_BENCHMARKS_TOOLCHAIN "LLVM"
#endif

#define ICE_BENCHMARKS_ASSERT(expression) \
  if (!(expression)) {                    \
    std::abort();                         \
  }

#ifdef SYMBOLS_EXPORTS
#  define SYMBOLS_API ICE_EXPORT
#else
#  define SYMBOLS_API ICE_IMPORT
#endif

#define SYMBOLS_FUNCTION(signature) \
  namespace external {              \
  SYMBOLS_API signature noexcept;   \
  }                                 \
  namespace internal {              \
  signature noexcept;               \
  }                                 \
  inline signature noexcept

extern "C" SYMBOLS_API int native(int result);

namespace symbols {

constexpr std::string_view string_data{
  "ee26b0dd4af7e749aa1a8ee3c10ae9923f618980772e473f8819a5d4940e0db2"
  "7ac185f8a0e1d5f84f88bc887fd67b143732c304cc5fa9ad8e6f57f50028a8ff"
};

using string = std::array<char, string_data.size()>;

SYMBOLS_FUNCTION(int code(int success))
{
  return success ? 0 : 1;
}

SYMBOLS_FUNCTION(ice::error_code<std::errc> error_code(int success))
{
  return success ? ice::make_error_code<std::errc>(0) : std::errc::no_such_file_or_directory;
}

SYMBOLS_FUNCTION(ice::error error(int success))
{
  return success ? ice::make_error<std::errc>(0) : std::errc::no_such_file_or_directory;
}

SYMBOLS_FUNCTION(ice::result<void> result_void(int success))
{
  if (success) {
    return {};
  }
  return std::errc::no_such_file_or_directory;
}

SYMBOLS_FUNCTION(ice::result<int> result_int(int success))
{
  if (success) {
    return success;
  }
  return std::errc::no_such_file_or_directory;
}

SYMBOLS_FUNCTION(ice::result<string> result_string(int success))
{
  if (success) {
    string string;
    std::copy(string_data.begin(), string_data.end(), string.begin());
    return string;
  }
  return std::errc::no_such_file_or_directory;
}

#if ICE_EXCEPTIONS

namespace external {

SYMBOLS_API void exceptions_void(int success);
SYMBOLS_API int exceptions_int(int success);
SYMBOLS_API string exceptions_string(int success);

}  // namespace external

namespace internal {

void exceptions_void(int success);
int exceptions_int(int success);
string exceptions_string(int success);

}  // namespace internal

inline void exceptions_void(int success)
{
  if (!success) {
    throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory));
  }
}

inline int exceptions_int(int success)
{
  if (!success) {
    throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory));
  }
  return success;
}

inline string exceptions_string(int success)
{
  if (!success) {
    throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory));
  }
  string string;
  std::copy(string_data.begin(), string_data.end(), string.begin());
  return string;
}

#endif

}  // namespace symbols
