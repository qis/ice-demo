#pragma once
#include <ice/config.hpp>

#if ICE_EXCEPTIONS
#  include <cstdlib>
#endif

#if __has_include(<coroutine>)
#  if defined(__clang__) && !defined(__cpp_lib_coroutine)
#    define __cpp_lib_coroutine 201902L
#  endif
#  include <coroutine>

namespace std::experimental {

using ::std::coroutine_handle;
using ::std::coroutine_traits;
using ::std::suspend_always;
using ::std::suspend_never;

}  // namespace std::experimental

#elif __has_include(<experimental/coroutine>)
#  include <experimental/coroutine>

namespace std {

using ::std::experimental::coroutine_handle;
using ::std::experimental::coroutine_traits;
using ::std::experimental::suspend_always;
using ::std::experimental::suspend_never;

}  // namespace std

#else
#  error Cannot include <coroutine> or <experimental/coroutine>.
#endif

namespace ice {

using std::coroutine_handle;
using std::coroutine_traits;
using std::suspend_always;
using std::suspend_never;

// ================================================================================================
// promise base
// ================================================================================================

struct promise_base {
#if ICE_EXCEPTIONS
  [[noreturn]] static void unhandled_exception() noexcept
  {
    std::abort();
  }
#else
  static constexpr void unhandled_exception() noexcept = delete;
#endif
};

}  // namespace ice
