#pragma once
#include "context.hpp"
#include <ice/application.hpp>
#include <ice/os/window.hpp>
#include <mutex>

namespace ice::os::xcb {

class window final : public ice::os::window, public std::enable_shared_from_this<window> {
public:
  ~window() override
  {
    window_.store(nullptr, std::memory_order_release);
    if (wm_delete_window_) {
      free(wm_delete_window_);
    }
    if (connection_) {
      if (id_) {
        xcb_destroy_window(connection_, id_);
      }
      xcb_disconnect(connection_);
    }
  }

  void move(ice::window* window) noexcept override
  {
    window_.store(window, std::memory_order_release);
    if (!window) {
      destroy();
    }
  }

  ice::error create() noexcept override
  {
    if (id_) {
      return {};
    }

    int screen_number = 0;
    connection_ = xcb_connect(nullptr, &screen_number);
    if (!connection_) {
      ICE_TRACE_FORMAT("xcb_connect: nullptr");
      return ice::errc::not_available;
    }
    if (const auto code = xcb_connection_has_error(connection_)) {
      auto e = ice::make_error<ice::os::xcb::errc>(code);
      ICE_TRACE_FORMAT("xcb_connect: {}", e);
      return e;
    }

    xcb_screen_t* screen = nullptr;
    const auto setup = xcb_get_setup(connection_);
    for (auto it = xcb_setup_roots_iterator(setup); it.rem; --screen_number, xcb_screen_next(&it)) {
      if (screen_number == 0) {
        screen = it.data;
        break;
      }
    }
    if (!screen) {
      ICE_TRACE_FORMAT("Could not find current screen.");
      screen = xcb_setup_roots_iterator(setup).data;
    }

    const auto id = xcb_generate_id(connection_);

    uint32_t event_mask = 0;
    event_mask |= XCB_EVENT_MASK_BUTTON_1_MOTION;
    event_mask |= XCB_EVENT_MASK_BUTTON_2_MOTION;
    event_mask |= XCB_EVENT_MASK_BUTTON_3_MOTION;
    event_mask |= XCB_EVENT_MASK_BUTTON_4_MOTION;
    event_mask |= XCB_EVENT_MASK_BUTTON_5_MOTION;
    event_mask |= XCB_EVENT_MASK_BUTTON_MOTION;
    event_mask |= XCB_EVENT_MASK_BUTTON_PRESS;
    event_mask |= XCB_EVENT_MASK_BUTTON_RELEASE;
    event_mask |= XCB_EVENT_MASK_EXPOSURE;
    event_mask |= XCB_EVENT_MASK_KEY_PRESS;
    event_mask |= XCB_EVENT_MASK_KEY_RELEASE;
    event_mask |= XCB_EVENT_MASK_KEYMAP_STATE;
    event_mask |= XCB_EVENT_MASK_POINTER_MOTION;
    event_mask |= XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    // clang-format off
    const uint8_t        d    = screen->root_depth;
    const xcb_window_t   p    = screen->root;
    const int16_t        x    = 0;
    const int16_t        y    = 0;
    const uint16_t       w    = 640;
    const uint16_t       h    = 480;
    const uint16_t       bw   = 0;
    const uint16_t       wc   = XCB_WINDOW_CLASS_INPUT_OUTPUT;
    const xcb_visualid_t v    = screen->root_visual;
    const uint32_t       m    = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    const uint32_t       mv[] = { 0x1E1E1E, event_mask };
    // clang-format on

    const auto cookie = xcb_create_window_checked(connection_, d, id, p, x, y, w, h, bw, wc, v, m, mv);
    if (const auto error = xcb_request_check(connection_, cookie)) {
      return ice::make_error<ice::os::xcb::errc>(error->error_code);
    }
    id_ = id;

    const auto wm_protocols_cookie = xcb_intern_atom(connection_, 1, 12, "WM_PROTOCOLS");
    const auto wm_protocols = xcb_intern_atom_reply(connection_, wm_protocols_cookie, 0);
    const auto wm_delete_window_cookie = xcb_intern_atom(connection_, 0, 16, "WM_DELETE_WINDOW");
    wm_delete_window_ = xcb_intern_atom_reply(connection_, wm_delete_window_cookie, 0);
    xcb_change_property(connection_, XCB_PROP_MODE_REPLACE, id, wm_protocols->atom, 4, 32, 1, &wm_delete_window_->atom);
    xcb_flush(connection_);
    free(wm_protocols);

    self_ = shared_from_this();
    if (auto window = window_.load(std::memory_order_acquire)) {
      window->on_create();
    }
    return {};
  }

  ice::error destroy() noexcept override
  {
    if (!id_) {
      return ice::errc::not_initialized;
    }
    if (auto window = window_.load(std::memory_order_acquire)) {
      window->on_destroy();
    }
    const auto id = std::exchange(id_, 0);
    const auto cookie = xcb_destroy_window_checked(connection_, id);
    if (const auto error = xcb_request_check(connection_, cookie)) {
      return ice::make_error<errc>(error->error_code);
    }
    return {};
  }

  ice::error set(ice::window::mode mode) noexcept override
  {
    if (!id_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error set(ice::window::style style) noexcept override
  {
    if (!id_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error text(std::string_view text) noexcept override
  {
    if (!id_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error icon(std::string_view icon) noexcept override
  {
    if (!id_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error start(std::string_view name, int size, int weight, ice::ui::font::flags flags) noexcept override
  {
    if (!id_) {
      return ice::errc::not_initialized;
    }
    auto xcb = std::make_unique<ice::os::xcb::context>();
    if (auto e = xcb->create(connection_, name, size, weight, flags)) {
      ICE_TRACE_FORMAT("Could not create XCB context: {}", e);
      return e;
    }
    context_ = std::move(xcb);
    return {};
  }

  ice::error show() noexcept override
  {
    if (!id_) {
      return ice::errc::not_initialized;
    }
    const auto cookie = xcb_map_window_checked(connection_, id_);
    if (const auto error = xcb_request_check(connection_, cookie)) {
      return ice::make_error<ice::os::xcb::errc>(error->error_code);
    }
    xcb_flush(connection_);
    return {};
  }

  ice::error hide() noexcept override
  {
    if (!id_) {
      return ice::errc::not_initialized;
    }
    const auto cookie = xcb_unmap_window_checked(connection_, id_);
    if (const auto error = xcb_request_check(connection_, cookie)) {
      return ice::make_error<ice::os::xcb::errc>(error->error_code);
    }
    xcb_flush(connection_);
    return {};
  }

  ice::error minimize() noexcept override
  {
    if (!id_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error maximize() noexcept override
  {
    if (!id_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error restore() noexcept override
  {
    if (!id_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error run() noexcept override
  {
    if (!id_) {
      return ice::errc::not_initialized;
    }
    xcb_generic_event_t* event = nullptr;
    while (id_ && (event = xcb_wait_for_event(connection_))) {
      handle(event);
      free(event);
      while (id_ && (event = xcb_poll_for_event(connection_))) {
        handle(event);
        free(event);
      }
    }
    ice::error e;
    if (!event) {
      if (const auto code = xcb_connection_has_error(connection_)) {
        e = ice::make_error<ice::os::xcb::errc>(code);
        ICE_TRACE_FORMAT("xcb_wait_for_event: {}", e);
      }
    }
    self_.reset();
    return e;
  }

  void handle(xcb_generic_event_t* event) noexcept
  {
    switch (XCB_EVENT_RESPONSE_TYPE(event)) {
    case XCB_CLIENT_MESSAGE: {
      const auto e = reinterpret_cast<const xcb_client_message_event_t*>(event);
      if (e->window == id_ && e->data.data32[0] == wm_delete_window_->atom) {
        if (auto window = window_.load(std::memory_order_acquire)) {
          window->on_close();
        } else {
          destroy();
        }
      }
    } break;
    case XCB_EXPOSE:
    case XCB_REPARENT_NOTIFY:
    case XCB_MAP_NOTIFY: {
      if (context_) {
        context_->render();
      }
    } break;
    case XCB_CONFIGURE_NOTIFY: {
      const auto e = reinterpret_cast<xcb_configure_notify_event_t*>(event);
      if (context_) {
        context_->resize(e->width, e->height);
      }
    } break;
    default:
      if (context_) {
        context_->handle(event);
      }
      break;
    }
  }

private:
  std::shared_ptr<window> self_;
  std::atomic<ice::window*> window_;
  std::unique_ptr<ice::os::xcb::context> context_;

  xcb_window_t id_{};
  xcb_connection_t* connection_{};
  xcb_intern_atom_reply_t* wm_delete_window_{};
};

}  // namespace ice::os::xcb
