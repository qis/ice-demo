#pragma once
#include "config.hpp"
#include <ice/application.hpp>
#include <ice/sys/window.hpp>

namespace ice::sys::xcb {

class window final : public ice::sys::window, public std::enable_shared_from_this<window> {
public:
  void set(ice::window* window) noexcept override
  {
    window_.store(window, std::memory_order_release);
  }

  ice::error create(std::string_view text, std::string_view icon) noexcept override
  {
    return ice::errc::not_implemented;
  }

  ice::error update(std::string_view text, std::string_view icon) noexcept override
  {
    return ice::errc::not_implemented;
  }

  ice::error destroy() noexcept override
  {
    return ice::errc::not_implemented;
  }

  ice::error show() noexcept override
  {
    return ice::errc::not_implemented;
  }

  ice::error hide() noexcept override
  {
    return ice::errc::not_implemented;
  }

  ice::error minimize() noexcept override
  {
    return ice::errc::not_implemented;
  }

  ice::error maximize() noexcept override
  {
    return ice::errc::not_implemented;
  }

  ice::error restore() noexcept override
  {
    return ice::errc::not_implemented;
  }

  ice::error set(ice::window::mode mode) noexcept override
  {
    return ice::errc::not_implemented;
  }

  ice::error set(ice::window::style style) noexcept override
  {
    return ice::errc::not_implemented;
  }

  ice::error run() noexcept override
  {
    auto system = shared_from_this();
    return ice::errc::not_implemented;
  }

private:
  std::atomic<ice::window*> window_{};

#if 0
  ~window() override
  {
    fmt::print(stderr, "window::~window\n");
  }

  ice::error create(ice::window* target, ice::sys::application_ptr application, std::string_view text) noexcept override
  {
    fmt::print(stderr, "window::create\n");
    ICE_ASSERT(application);
    ICE_ASSERT(application->platform() == ice::platform::xcb);
    application_ = std::static_pointer_cast<ice::sys::xcb::application>(application);
    target_.store(target, std::memory_order_release);
    connection_ = application_->connection();

    window_id_ = xcb_generate_id(connection_);
    const auto screen = xcb_setup_roots_iterator(xcb_get_setup(connection_)).data;
    ICE_ASSERT(screen);

    // clang-format off
    const uint8_t        d    = screen->root_depth;
    const xcb_window_t   p    = screen->root;
    const int16_t        x    = 0;
    const int16_t        y    = 0;
    const uint16_t       w    = 1280;
    const uint16_t       h    = 720;
    const uint16_t       bw   = 0;
    const uint16_t       wc   = XCB_WINDOW_CLASS_INPUT_OUTPUT;
    const xcb_visualid_t v    = screen->root_visual;
    const uint32_t       m    = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    const uint32_t       mv[] = { screen->black_pixel, XCB_EVENT_MASK_EXPOSURE };
    // clang-format on

    const auto cookie = xcb_create_window_checked(connection_, d, window_id_, p, x, y, w, h, bw, wc, v, m, mv);
    if (const auto error = xcb_request_check(connection_, cookie)) {
      return ice::make_error<errc>(error->error_code);
    }

    // clang-format off
    //const auto wm_protocols_cookie = xcb_intern_atom(connection_, 1, 12, "WM_PROTOCOLS");
    //const auto wm_protocols = xcb_intern_atom_reply(connection_, wm_protocols_cookie, 0);
    //const auto wm_delete_window_cookie = xcb_intern_atom(connection_, 0, 16, "WM_DELETE_WINDOW");
    //const auto wm_delete_window = xcb_intern_atom_reply(connection_, wm_delete_window_cookie, 0);
    //xcb_change_property(connection_, XCB_PROP_MODE_REPLACE, window_id_, wm_protocols->atom, 4, 32, 1, &wm_delete_window->atom);
    //free(wm_protocols);  // NOLINT
    // clang-format on

    show();

    const auto dummy = xcb_generate_id(connection_);
    xcb_create_window(connection_, d, dummy, p, x, y, 100, 100, bw, wc, v, m, mv);
    xcb_map_window(connection_, dummy);
    xcb_flush(connection_);

    //window_ = shared_from_this();  // reset after on_destroy was called
    return {};
  }

  ice::error destroy() noexcept override
  {
    fmt::print(stderr, "window::destroy\n");
    const auto cookie = xcb_destroy_window_checked(connection_, window_id_);
    if (const auto error = xcb_request_check(connection_, cookie)) {
      return ice::make_error<errc>(error->error_code);
    }
    xcb_flush(connection_);
    return {};
  }

  ice::error close() noexcept override
  {
    return destroy();
  }

  ice::error show() noexcept override
  {
    fmt::print(stderr, "window::show\n");
    const auto cookie = xcb_map_window_checked(connection_, window_id_);
    if (const auto error = xcb_request_check(connection_, cookie)) {
      return ice::make_error<errc>(error->error_code);
    }
    xcb_flush(connection_);
    return {};
  }

  ice::error hide() noexcept override
  {
    fmt::print(stderr, "window::hide\n");
    const auto cookie = xcb_unmap_window_checked(connection_, window_id_);
    if (const auto error = xcb_request_check(connection_, cookie)) {
      return ice::make_error<errc>(error->error_code);
    }
    xcb_flush(connection_);
    return {};
  }

  void set(ice::window* target) noexcept override
  {
    target_.store(target, std::memory_order_release);
  }

  ice::sys::application_ptr application() const noexcept override
  {
    return application_;
  }

private:
  ice::sys::window_ptr window_;
  std::atomic<ice::window*> target_{ nullptr };
  std::shared_ptr<ice::sys::xcb::application> application_;
  xcb_connection_t* connection_{ nullptr };
  xcb_window_t window_id_{ 0 };
#endif
};


}  // namespace ice::sys::xcb
