#pragma once
#include <ice/config.hpp>
#include <utility>

namespace ice {

// ================================================================================================
// scope exit
// ================================================================================================

template <typename Handler>
class scope_exit {
public:
  explicit constexpr scope_exit(Handler handler) noexcept
    : handler_(std::move(handler))
  {}

  scope_exit(scope_exit&& other) = delete;
  scope_exit(const scope_exit& other) = delete;
  scope_exit& operator=(scope_exit&& other) = delete;
  scope_exit& operator=(const scope_exit& other) = delete;

  constexpr ~scope_exit()
  {
    if (!cancelled_) {
      handler_();
    }
  }

  constexpr void cancel(bool cancel = true) noexcept
  {
    cancelled_ = cancel;
  }

private:
  Handler handler_;
  bool cancelled_{ false };
};

template <typename Handler>
[[nodiscard]] constexpr auto on_scope_exit(Handler&& handler) noexcept
{
  return scope_exit<Handler>{ std::forward<Handler>(handler) };
}

// ================================================================================================
// thread local storage
// ================================================================================================

class ICE_API thread_local_storage {
public:
#ifdef _WIN32
  static constexpr std::uintptr_t invalid_handle_value = static_cast<std::uintptr_t>(-1);
#else
  static constexpr std::uintptr_t invalid_handle_value = 0;
#endif

  thread_local_storage() noexcept;
  thread_local_storage(thread_local_storage&& other) = delete;
  thread_local_storage(const thread_local_storage& other) = delete;
  thread_local_storage& operator=(thread_local_storage&& other) = delete;
  thread_local_storage& operator=(const thread_local_storage& other) = delete;

  ~thread_local_storage();

  bool set(void* value) noexcept;
  void* get() const noexcept;

private:
  std::uintptr_t handle_ = invalid_handle_value;
};

}  // namespace ice
