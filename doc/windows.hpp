#pragma once
#if 0
#include "config.hpp"
#include <ice/application.hpp>
#include <atomic>

namespace ice::sys::windows {

// ================================================================================================
// application
// ================================================================================================

class application final : public ice::sys::application {
public:
  ice::error create() noexcept override
  {
    std::thread::id expected;
    if (!thread_.compare_exchange_strong(expected, std::this_thread::get_id())) {
      return ice::application::errc::already_initialized;
    }
    native_thread_ = GetCurrentThreadId();
    return {};
  }

  ice::error shutdown(int code) noexcept override
  {
    const auto thread = thread_.load(std::memory_order_acquire);
    if (thread == std::thread::id{}) {
      return ice::application::errc::not_initialized;
    }
    fmt::print("posting code: {}\n", code);
    if (!PostThreadMessage(native_thread_, WM_ICE_APPLICATION_SHUTDOWN, static_cast<WPARAM>(code), 0)) {
      return get_last_error();
    }
    return {};
  }

  int run() noexcept override
  {
    const auto thread = thread_.load(std::memory_order_acquire);
    ICE_ASSERT(thread == std::this_thread::get_id());
    if (thread != std::this_thread::get_id()) {
      return EXIT_FAILURE;
    }

    // Process messages until shutdown requested.
    MSG msg = {};
    int ret = EXIT_SUCCESS;
    while (GetMessage(&msg, nullptr, 0, 0)) {
      if (msg.message == WM_ICE_APPLICATION_SHUTDOWN) {
        ret = static_cast<int>(msg.wParam);
        break;
      }
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // Process queued messages after shutdown requested.
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // Indicate to the system that a thread has made a request to terminate.
    PostQuitMessage(ret);

    // Process messages queued while `PostQuitMessage(EXIT_SUCCESS)` was running.
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
  }

  std::thread::id thread() const noexcept override
  {
    return thread_.load(std::memory_order_acquire);
  }

private:
  std::atomic<std::thread::id> thread_{};
  DWORD native_thread_{ 0 };
};

}  // namespace ice::sys::windows
#endif
