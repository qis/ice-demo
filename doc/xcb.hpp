#pragma once
#include "config.hpp"
#if 0
#include <ice/application.hpp>
#include <atomic>

namespace ice::sys::xcb {

// ================================================================================================
// application
// ================================================================================================

class application final : public ice::sys::application {
public:
  ~application() override
  {
    if (connection_) {
      fmt::print(stderr, "application::~application\n");
      xcb_disconnect(connection_);
      connection_ = nullptr;
    }
  }

  ice::error create() noexcept override
  {
    fmt::print(stderr, "application::create\n");
    std::thread::id expected;
    if (!thread_.compare_exchange_strong(expected, std::this_thread::get_id())) {
      return ice::application::errc::already_initialized;
    }
    connection_ = xcb_connect(nullptr, nullptr);
    if (!connection_) {
      return ice::application::errc::platform_not_available;
    }
    if (const auto code = xcb_connection_has_error(connection_)) {
      return static_cast<errc>(code);
    }
    return {};
  }

  ice::error shutdown(int code) noexcept override
  {
    fmt::print(stderr, "application::shutdown\n");
    bool shutdown = false;
    if (!shutdown_.compare_exchange_strong(shutdown, true)) {
      return ice::application::errc::not_initialized;
    }
    ICE_ASSERT(connection_);
    code_.store(code, std::memory_order_release);
    xcb_client_message_event_t event{};
    event.response_type = XCB_CLIENT_MESSAGE;
    event.window = xcb_generate_id(connection_);
    event.format = 32;
    xcb_send_event(connection_, 0, event.window, 0, reinterpret_cast<const char*>(&event));
    xcb_flush(connection_);
    return {};
  }

  int run() noexcept override
  {
    fmt::print(stderr, "application::run\n");
    const auto thread = thread_.load(std::memory_order_acquire);
    ICE_ASSERT(thread == std::this_thread::get_id());
    if (thread != std::this_thread::get_id()) {
      return EXIT_FAILURE;
    }

    ICE_ASSERT(connection_);

    int ret = EXIT_SUCCESS;
    while (!shutdown_.load(std::memory_order_acquire)) {
      const auto event = xcb_wait_for_event(connection_);
      if (!event) {
        if (const auto code = xcb_connection_has_error(connection_)) {
          fmt::print(stderr, "application::run: no event, error: {}\n", make_error<errc>(code));
          return code;
        }
        fmt::print(stderr, "application::run: no event, no error\n");
        continue;
      }
      //fmt::print(stderr, "application::run: event\n");

      // TODO: Process (event->response_type & ~0x80) here!

      free(event);  // NOLINT(cppcoreguidelines-no-malloc)
    }
    fmt::print(stderr, "application::run: done\n");

    //while (true) {
    //  const auto event = xcb_poll_for_event(connection_);
    //  if (!event) {
    //    if (const auto code = xcb_connection_has_error(connection_)) {
    //      return code;
    //    }
    //    break;
    //  }
    //  // TODO: Process (event->response_type & ~0x80) here!
    //  free(event);  // NOLINT(cppcoreguidelines-no-malloc)
    //}

    return code_.load(std::memory_order_acquire);
  }

  std::thread::id thread() const noexcept override
  {
    return thread_.load(std::memory_order_acquire);
  }

  constexpr xcb_connection_t* connection() const noexcept
  {
    return connection_;
  }

private:
  std::atomic_bool shutdown_{ false };
  std::atomic<std::thread::id> thread_{};
  std::atomic<int> code_{ EXIT_SUCCESS };
  xcb_connection_t* connection_{ nullptr };
};

}  // namespace ice::sys::xcb
#endif
