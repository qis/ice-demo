#pragma once
#include <system_error>

// ================================================================================================
// config
// ================================================================================================

#ifndef ICE_DEBUG
#  ifdef NDEBUG
#    define ICE_DEBUG 0
#  else
#    define ICE_DEBUG 1
#  endif
#endif

// ================================================================================================
// macros
// ================================================================================================

#ifdef _WIN32
#  define ICE_EXPORT __declspec(dllexport)
#  define ICE_IMPORT __declspec(dllimport)
#else
#  define ICE_EXPORT __attribute__((__visibility__("default")))
#  define ICE_IMPORT
#endif

#ifdef ICE_SHARED
#  ifdef ICE_EXPORTS
#    define ICE_API ICE_EXPORT
#  else
#    define ICE_API ICE_IMPORT
#  endif
#else
#  define ICE_API
#endif

#if !defined(__clang__) || defined(__cpp_exceptions)
#  define ICE_EXCEPTIONS 1
#else
#  define ICE_EXCEPTIONS 0
#endif

#ifdef __has_builtin
#  if __has_builtin(__builtin_expect)
#    define ICE_LIKELY(expr) __builtin_expect(expr, 1)
#  endif
#endif
#ifndef ICE_LIKELY
#  define ICE_LIKELY(expr) expr
#endif

#ifdef __has_builtin
#  if __has_builtin(__builtin_expect)
#    define ICE_UNLIKELY(expr) __builtin_expect(expr, 0)
#  endif
#endif
#ifndef ICE_UNLIKELY
#  define ICE_UNLIKELY(expr) expr
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#  define ICE_ALWAYS_INLINE __forceinline
#else
#  define ICE_ALWAYS_INLINE __attribute__((always_inline))
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#  define ICE_OFFSETOF(s, m) ((size_t)(&reinterpret_cast<char const volatile&>((((s*)0)->m))))
#else
#  define ICE_OFFSETOF __builtin_offsetof
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#  define ICE_FUNCTION __FUNCSIG__
#else
#  define ICE_FUNCTION __PRETTY_FUNCTION__
#endif

#if ICE_DEBUG
#  include <cassert>
#  define ICE_ASSERT(expression) assert(expression)
#else
#  define ICE_ASSERT(expression)
#endif

namespace ice {

// ================================================================================================
// errc
// ================================================================================================

enum class errc {
  success = 0,
  not_available,
  not_implemented,
  not_initialized,
  context_not_empty,
  invalid_result_value,
  unicode_buffer_too_small,
  unicode_incomplete_sequence,
  unicode_invalid_code_point,
  unicode_invalid_lead,
  unicode_invalid_options,
  unicode_invalid_utf8,
  unicode_not_enough_memory,
  unicode_overlong_sequence,
  unicode_unassigned,
  unknown = 0xFFFFFFF,
};

}  // namespace ice

namespace ice::system {

// ================================================================================================
// errc
// ================================================================================================

enum class errc;

}  // namespace ice::system
