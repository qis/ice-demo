#pragma once
#include <ice/coroutine.hpp>
#include <ice/error.hpp>

namespace ice {

// ================================================================================================
// task
// ================================================================================================

struct task {
  struct promise_type : ice::promise_base {
    static constexpr auto get_return_object() noexcept
    {
      return task{};
    }

    static constexpr auto initial_suspend() noexcept
    {
      return ice::suspend_never{};
    }

    static constexpr auto final_suspend() noexcept
    {
      return ice::suspend_never{};
    }

    static constexpr void return_void() noexcept
    {}

    [[noreturn]] static ICE_API void return_error(ice::error error) noexcept;
  };
};

}  // namespace ice
