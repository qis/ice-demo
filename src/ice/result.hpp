#pragma once
#include <ice/coroutine.hpp>
#include <ice/error.hpp>
#include <type_traits>
#include <utility>

namespace ice {

// ================================================================================================
// result value type
// ================================================================================================

// clang-format off

template <typename T>
concept ResultValueType =
  !ErrorCodeType<std::remove_cvref_t<T>> &&
  !ErrorCode<std::remove_cvref_t<T>> &&
  !Error<std::remove_cvref_t<T>>;

// clang-format on

// ================================================================================================
// result
// ================================================================================================

template <ResultValueType T>
class result {
public:
  using value_type = std::remove_cvref_t<T>;

  struct promise_type : ice::promise_base {
    constexpr result get_return_object() noexcept
    {
      return result{ this };
    }

    static constexpr auto initial_suspend() noexcept
    {
      return ice::suspend_never{};
    }

    static constexpr auto final_suspend() noexcept
    {
      return ice::suspend_never{};
    }

    template <typename Arg, typename... Args>
    constexpr void return_value(Arg&& arg, Args&&... args) noexcept
    {
      if constexpr (Error<Arg> || ErrorCode<Arg> || ErrorCodeType<Arg>) {
        result_ptr->return_error({ std::forward<Arg>(arg), std::forward<Args>(args)... });
      } else {
        result_ptr->return_value(std::forward<Arg>(arg), std::forward<Args>(args)...);
      }
    }

    constexpr void return_value(value_type rv) noexcept requires(std::is_move_constructible_v<value_type>)
    {
      result_ptr->return_value(std::move(rv));
    }

    constexpr void return_error(ice::error e) noexcept
    {
      result_ptr->return_error(e);
    }

    result* result_ptr = nullptr;
  };

  explicit constexpr result(promise_type* promise) noexcept
    : error_(ice::errc::invalid_result_value)
  {
    promise->result_ptr = this;
  }

  template <ErrorCodeType E>
  constexpr result(E ev) noexcept
    : error_(ICE_LIKELY(static_cast<int>(ev)) ? make_error<E>(ev) : make_error(ice::errc::invalid_result_value))
  {}

  template <ErrorCodeType E>
  constexpr result(ice::error_code<E> ec) noexcept
    : error_(ICE_LIKELY(static_cast<bool>(ec)) ? make_error(ec) : make_error(ice::errc::invalid_result_value))
  {}

  constexpr result(ice::error e) noexcept
    : error_(ICE_LIKELY(static_cast<bool>(e)) ? e : make_error(ice::errc::invalid_result_value))
  {}

  result(std::error_code ec) noexcept
    : error_(ICE_LIKELY(static_cast<bool>(ec)) ? make_error(ec) : make_error(ice::errc::invalid_result_value))
  {}

  template <ResultValueType Value, typename... Args>
  constexpr result(Value&& rv, Args&&... args) noexcept
  {
    new (static_cast<void*>(&value_)) value_type(std::forward<Value>(rv), std::forward<Args>(args)...);
  }

  result() requires(!std::is_default_constructible_v<value_type>) = delete;

  constexpr result() noexcept requires(std::is_default_constructible_v<value_type>)
  {
    new (static_cast<void*>(&value_)) value_type();
  }

  result(result&& other) requires(!std::is_move_constructible_v<value_type>) = delete;

  constexpr result(result&& other) noexcept requires(std::is_move_constructible_v<value_type>)
    : error_(other.error_)
  {
    if (!ICE_UNLIKELY(static_cast<bool>(error_))) {
      new (static_cast<void*>(&value_)) value_type(std::move(other.value_));
    }
  }

  result(const result& other) requires(!std::is_copy_constructible_v<value_type>) = delete;

  constexpr result(const result& other) noexcept requires(std::is_copy_constructible_v<value_type>)
    : error_(other.error_)
  {
    if (!ICE_UNLIKELY(static_cast<bool>(error_))) {
      new (static_cast<void*>(&value_)) value_type(other.value_);
    }
  }

  result& operator=(result&& other) requires(!std::is_move_assignable_v<value_type>) = delete;

  constexpr result& operator=(result&& other) noexcept requires(std::is_move_assignable_v<value_type>)
  {
    if (!ICE_UNLIKELY(static_cast<bool>(error_))) {
      value_.~value_type();
    }
    error_ = other.error_;
    if (!ICE_UNLIKELY(static_cast<bool>(error_))) {
      new (static_cast<void*>(&value_)) value_type(std::move(other.value_));
    }
    return *this;
  }

  result& operator=(const result& other) requires(!std::is_copy_assignable_v<value_type>) = delete;

  constexpr result& operator=(const result& other) noexcept requires(std::is_copy_assignable_v<value_type>)
  {
    if (!ICE_UNLIKELY(static_cast<bool>(error_))) {
      value_.~value_type();
    }
    error_ = other.error_;
    if (!ICE_UNLIKELY(static_cast<bool>(error_))) {
      new (static_cast<void*>(&value_)) value_type(other.value_);
    }
    return *this;
  }

  constexpr ~result()
  {
    if (!ICE_UNLIKELY(static_cast<bool>(error_))) {
      value_.~value_type();
    }
  }

  constexpr explicit operator bool() const noexcept
  {
    return !error_;
  }

  constexpr value_type* operator->() noexcept
  {
    ICE_ASSERT(!error_);
    return &value_;
  }

  constexpr const value_type* operator->() const noexcept
  {
    ICE_ASSERT(!error_);
    return &value_;
  }

  constexpr value_type& operator*() & noexcept
  {
    ICE_ASSERT(!error_);
    return value_;
  }

  constexpr value_type&& operator*() && noexcept
  {
    ICE_ASSERT(!error_);
    return std::move(value_);
  }

  constexpr const value_type& operator*() const& noexcept
  {
    ICE_ASSERT(!error_);
    return value_;
  }

  constexpr const value_type&& operator*() const&& noexcept
  {
    ICE_ASSERT(!error_);
    return value_;
  }

  constexpr value_type& value() & noexcept
  {
    ICE_ASSERT(!error_);
    return value_;
  }

  constexpr value_type&& value() && noexcept
  {
    ICE_ASSERT(!error_);
    return std::move(value_);
  }

  constexpr const value_type& value() const& noexcept
  {
    ICE_ASSERT(!error_);
    return value_;
  }

  constexpr const value_type&& value() const&& noexcept
  {
    ICE_ASSERT(!error_);
    return std::move(value_);
  }

  constexpr ice::error error() const noexcept
  {
    return error_;
  }

  constexpr ice::error_type type() const noexcept
  {
    return error_.type();
  }

  constexpr int code() const noexcept
  {
    return error_.code();
  }

  constexpr bool await_ready() const noexcept
  {
    return !error_;
  }

  template <typename Promise>
  constexpr bool await_suspend(ice::coroutine_handle<Promise> handle) noexcept
  {
    ICE_ASSERT(error_);
    handle.promise().return_error(error());
    return true;
  }

  constexpr void await_resume() const noexcept
  {
    ICE_ASSERT(error_);
  }

private:
  template <typename... Args>
  constexpr void return_value(Args&&... args) noexcept
  {
    new (static_cast<void*>(&value_)) value_type(std::forward<Args>(args)...);
    error_ = {};
  }

  constexpr void return_error(ice::error e) noexcept
  {
    error_ = ICE_LIKELY(static_cast<bool>(e)) ? e : make_error(ice::errc::invalid_result_value);
  }

  ice::error error_;
  union {
    value_type value_;  // NOLINT(readability-identifier-naming)
  };
};

// ------------------------------------------------------------------------------------------------
// result (void specialization)
// ------------------------------------------------------------------------------------------------

template <>
class result<void> {
public:
  using value_type = void;

  struct promise_type : ice::promise_base {
    constexpr result get_return_object() noexcept
    {
      return result{ this };
    }

    static constexpr auto initial_suspend() noexcept
    {
      return ice::suspend_never{};
    }

    static constexpr auto final_suspend() noexcept
    {
      return ice::suspend_never{};
    }

    constexpr void return_value(ice::error e) const noexcept
    {
      result_ptr->return_value(e);
    }

    constexpr void return_error(ice::error e) const noexcept
    {
      result_ptr->return_error(e);
    }

    result* result_ptr = nullptr;
  };

  explicit constexpr result(promise_type* promise) noexcept
    : error_(ice::errc::invalid_result_value)
  {
    promise->result_ptr = this;
  }

  template <ErrorCodeType E>
  constexpr result(E ev) noexcept
    : error_(ICE_LIKELY(static_cast<int>(ev)) ? make_error<E>(ev) : make_error(ice::errc::invalid_result_value))
  {}

  template <ErrorCodeType E>
  constexpr result(ice::error_code<E> ec) noexcept
    : error_(ICE_LIKELY(static_cast<bool>(ec)) ? make_error(ec) : make_error(ice::errc::invalid_result_value))
  {}

  constexpr result(ice::error e) noexcept
    : error_(ICE_LIKELY(static_cast<bool>(e)) ? e : make_error(ice::errc::invalid_result_value))
  {}

  result(std::error_code ec) noexcept
    : error_(ICE_LIKELY(static_cast<bool>(ec)) ? make_error(ec) : make_error(ice::errc::invalid_result_value))
  {}

  result() noexcept = default;
  result(result&& other) noexcept = default;
  result(const result& other) noexcept = default;
  result& operator=(result&& other) noexcept = default;
  result& operator=(const result& other) noexcept = default;

#ifndef _MSC_VER
  constexpr ~result()  // NOLINT(modernize-use-equals-default)
  {}
#endif

  constexpr explicit operator bool() const noexcept
  {
    return !error_;
  }

  constexpr value_type operator*() const noexcept
  {}

  constexpr value_type value() const noexcept
  {}

  constexpr ice::error error() const noexcept
  {
    return error_;
  }

  constexpr ice::error_type type() const noexcept
  {
    return error_.type();
  }

  constexpr int code() const noexcept
  {
    return error_.code();
  }

  constexpr bool await_ready() const noexcept
  {
    return !error_;
  }

  template <typename Promise>
  constexpr bool await_suspend(ice::coroutine_handle<Promise> handle) noexcept
  {
    ICE_ASSERT(error_);
    handle.promise().return_error(error());
    return true;
  }

  constexpr void await_resume() const noexcept  // NOLINT(readability-convert-member-functions-to-static)
  {
    ICE_ASSERT(error_);
  }

private:
  constexpr void return_value(ice::error e) noexcept
  {
    error_ = e;
  }

  constexpr void return_error(ice::error e) noexcept
  {
    error_ = ICE_LIKELY(static_cast<bool>(e)) ? e : make_error(ice::errc::invalid_result_value);
  }

  ice::error error_;
};

template <ResultValueType T, ErrorCodeType E>
constexpr bool operator==(const ice::result<T>& lhs, E rhs) noexcept
{
  return lhs.error() == rhs;
}

template <ResultValueType T, ErrorCodeType E>
constexpr bool operator==(const ice::result<T>& lhs, ice::error_code<E> rhs) noexcept
{
  return lhs.error() == rhs;
}

template <ResultValueType T>
constexpr bool operator==(const ice::result<T>& lhs, ice::error rhs) noexcept
{
  return lhs.error() == rhs;
}

}  // namespace ice
