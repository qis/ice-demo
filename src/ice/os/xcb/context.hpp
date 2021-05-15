#pragma once
#include "config.hpp"
#include <ice/format.hpp>
#include <ice/os/nuklear.hpp>
#include <ice/ui/context.hpp>

namespace ice::os::xcb {

class context final : public ice::ui::context {
public:
  ~context() override
  {
    ice::ui::context::set(nullptr);
    if (context_.memory.memory.ptr) {
      nk_free(&context_);
    }
    if (key_symbols_) {
      xcb_key_symbols_free(key_symbols_);
    }
  }

  ice::error create(xcb_connection_t* c, std::string_view name, int size, int weight, ice::ui::font::flags flags) noexcept
  {
    if (key_symbols_) {
      ICE_TRACE_FORMAT("xcb_key_symbols_t: 0x{:016X}", reinterpret_cast<uintptr_t>(key_symbols_));
      return ice::errc::not_available;
    }

    key_symbols_ = xcb_key_symbols_alloc(c);
    if (const auto code = xcb_connection_has_error(c)) {
      auto e = ice::make_error<ice::os::xcb::errc>(code);
      ICE_TRACE_FORMAT("xcb_key_symbols_alloc: {}", e);
      return e;
    }
    if (!key_symbols_) {
      ICE_TRACE_FORMAT("xcb_key_symbols_alloc: nullptr");
      return ice::errc::not_available;
    }

    // if (hwnd_) {
    //   ICE_TRACE_FORMAT("HWND: 0x{:016X}", reinterpret_cast<uintptr_t>(hwnd_));
    //   return ice::errc::not_available;
    // }

    // hwnd_ = hwnd;
    // if (!GetClientRect(hwnd_, &rect_)) {
    //   auto e = ice::make_error<ice::system::errc>(GetLastError());
    //   ICE_TRACE_FORMAT("GetClientRect: {}", e);
    //   return e;
    // }

    // window_ = GetDC(hwnd_);
    // if (!window_) {
    //   ICE_TRACE_FORMAT("GetDC: NULL");
    //   return ice::errc::not_available;
    // }

    // memory_ = CreateCompatibleDC(window_);
    // if (!memory_) {
    //   ICE_TRACE_FORMAT("CreateCompatibleDC: NULL");
    //   return ice::errc::not_available;
    // }

    // memory_state_ = SaveDC(memory_);
    // if (!memory_state_) {
    //   ICE_TRACE_FORMAT("SaveDC: 0");
    //   return ice::errc::not_available;
    // }

    // bitmap_ = CreateCompatibleBitmap(window_, rect_.right, rect_.bottom);
    // if (!bitmap_) {
    //   ICE_TRACE_FORMAT("CreateCompatibleBitmap: 0");
    //   return ice::errc::not_available;
    // }
    // SelectObject(memory_, bitmap_);

    // nk_init_default(&context_, nullptr);
    // context_.clip.copy = copy;
    // context_.clip.paste = paste;
    ice::ui::context::set(&context_);
    return {};
  }

  std::shared_ptr<ice::ui::font> create_font(std::string_view name, int size, int weight = 400,
    ice::ui::font::flags flags = ice::ui::font::flags::normal) noexcept override
  {
    // auto font = std::make_shared<ice::os::xcb::context::font>();
    // if (auto e = font->create(window(), std::string{ name }, size, weight, flags)) {
    //   ICE_TRACE_FORMAT("Could not create GDI font: {}", e);
    // }
    // return font;
    return {};
  }

  void render() noexcept
  {
    // TODO
  }

  void resize(uint16_t cx, uint16_t cy) noexcept
  {
    // TODO
  }

  void handle(xcb_generic_event_t* event) noexcept
  {
    switch (const auto type = XCB_EVENT_RESPONSE_TYPE(event)) {
    case XCB_KEY_PRESS:
    case XCB_KEY_RELEASE: {
      const auto e = reinterpret_cast<xcb_key_press_event_t*>(event);
      const auto s = xcb_key_symbols_get_keysym(key_symbols_, e->detail, e->state);
      const auto down = type == XCB_KEY_PRESS;
      switch (s) {
      case XK_Shift_L:
      case XK_Shift_R:
        nk_input_key(&context_, NK_KEY_SHIFT, down);
        break;
      case XK_Control_L:
      case XK_Control_R:
        nk_input_key(&context_, NK_KEY_CTRL, down);
        break;
      case XK_Delete:
        nk_input_key(&context_, NK_KEY_DEL, down);
        break;
      case XK_Return:
        nk_input_key(&context_, NK_KEY_ENTER, down);
        break;
      case XK_Tab:
        nk_input_key(&context_, NK_KEY_TAB, down);
        break;
      case XK_BackSpace:
        nk_input_key(&context_, NK_KEY_BACKSPACE, down);
        break;
      case XK_Up:
        nk_input_key(&context_, NK_KEY_UP, down);
        break;
      case XK_Down:
        nk_input_key(&context_, NK_KEY_DOWN, down);
        break;
      case XK_Left:
        nk_input_key(&context_, NK_KEY_LEFT, down);
        break;
      case XK_Right:
        nk_input_key(&context_, NK_KEY_RIGHT, down);
        break;
      case XK_Escape:
        nk_input_key(&context_, NK_KEY_TEXT_RESET_MODE, down);
        break;
      case XK_Home: {
        nk_input_key(&context_, NK_KEY_TEXT_START, down);
        nk_input_key(&context_, NK_KEY_SCROLL_START, down);
      } break;
      case XK_End: {
        nk_input_key(&context_, NK_KEY_TEXT_END, down);
        nk_input_key(&context_, NK_KEY_SCROLL_END, down);
      } break;
      case XK_Page_Down:
        nk_input_key(&context_, NK_KEY_SCROLL_DOWN, down);
        break;
      case XK_Page_Up:
        nk_input_key(&context_, NK_KEY_SCROLL_UP, down);
        break;
      default:
        // clang-format off
        if (down &&
          !xcb_is_cursor_key(s) &&
          !xcb_is_function_key(s) &&
          !xcb_is_keypad_key(s) &&
          !xcb_is_misc_function_key(s) &&
          !xcb_is_modifier_key(s) &&
          !xcb_is_pf_key(s) &&
          !xcb_is_private_keypad_key(s))
        {
          nk_input_char(&context_, static_cast<char>(s));
        }
        // clang-format on
        break;
      }
    } break;
    case XCB_BUTTON_PRESS:
    case XCB_BUTTON_RELEASE: {
      const auto e = reinterpret_cast<xcb_button_press_event_t*>(event);
      const auto down = type == XCB_BUTTON_PRESS;
      switch (e->detail) {
      case XCB_BUTTON_INDEX_1:
        nk_input_button(&context_, NK_BUTTON_LEFT, e->event_x, e->event_y, down);
        break;
      case XCB_BUTTON_INDEX_2:
        nk_input_button(&context_, NK_BUTTON_MIDDLE, e->event_x, e->event_y, down);
        break;
      case XCB_BUTTON_INDEX_3:
        nk_input_button(&context_, NK_BUTTON_RIGHT, e->event_x, e->event_y, down);
        break;
      case XCB_BUTTON_INDEX_4:
        nk_input_scroll(&context_, nk_vec2(0, 1.0f));
        break;
      case XCB_BUTTON_INDEX_5:
        nk_input_scroll(&context_, nk_vec2(0, -1.0f));
        break;
      }
    } break;
    case XCB_MOTION_NOTIFY: {
      const auto e = reinterpret_cast<xcb_motion_notify_event_t*>(event);
      nk_input_motion(&context_, e->event_x, e->event_y);
    } break;
    case XCB_KEYMAP_NOTIFY: {
      const auto e = reinterpret_cast<xcb_mapping_notify_event_t*>(event);
      xcb_refresh_keyboard_mapping(key_symbols_, e);
    } break;
    }
  }

  bool begin(const char* title, struct nk_rect bounds, nk_flags flags) noexcept
  {
    return nk_begin(&context_, title, bounds, flags);
  }

  void end() noexcept
  {
    nk_end(&context_);
  }

  void clear() noexcept
  {
    nk_clear(&context_);
  }

  void input_begin() noexcept
  {
    nk_input_begin(&context_);
  }

  void input_end() noexcept
  {
    nk_input_end(&context_);
  }

  constexpr float cx() const noexcept override
  {
    return static_cast<float>(cx_);
  }

  constexpr float cy() const noexcept override
  {
    return static_cast<float>(cy_);
  }

  constexpr nk_context* get() noexcept
  {
    return &context_;
  }

private:
  xcb_key_symbols_t* key_symbols_{};
  nk_context context_{};
  float cx_{};
  float cy_{};

  static void copy(nk_handle user, const char* string, int length) noexcept
  {}

  static void paste(nk_handle user, nk_text_edit* edit) noexcept
  {}
};

}  // namespace ice::os::xcb
