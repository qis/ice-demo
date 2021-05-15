#pragma once
#include <ice/format.hpp>
#include <ice/os/nuklear.hpp>
#include <ice/ui/context.hpp>
#include <ice/utility.hpp>
#include <windows.h>

namespace ice::os::windows {

class context : public ice::ui::context {
public:
  ~context() override
  {
    ice::ui::context::set(nullptr);
    if (context_.memory.memory.ptr) {
      nk_free(&context_);
    }
    if (memory_) {
      RestoreDC(memory_, memory_state_);
    }
    if (bitmap_) {
      SelectObject(memory_, bitmap_);
    }
    if (memory_) {
      DeleteDC(memory_);
    }
    if (window_) {
      ReleaseDC(hwnd_, window_);
    }
  }

  ice::error create(HWND hwnd) noexcept
  {
    if (hwnd_) {
      ICE_TRACE_FORMAT("HWND: 0x{:016X}", reinterpret_cast<uintptr_t>(hwnd_));
      return ice::errc::not_available;
    }

    hwnd_ = hwnd;
    if (!GetClientRect(hwnd_, &rect_)) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("GetClientRect: {}", e);
      return e;
    }

    window_ = GetDC(hwnd_);
    if (!window_) {
      ICE_TRACE_FORMAT("GetDC: NULL");
      return ice::errc::not_available;
    }

    memory_ = CreateCompatibleDC(window_);
    if (!memory_) {
      ICE_TRACE_FORMAT("CreateCompatibleDC: NULL");
      return ice::errc::not_available;
    }

    memory_state_ = SaveDC(memory_);
    if (!memory_state_) {
      ICE_TRACE_FORMAT("SaveDC: 0");
      return ice::errc::not_available;
    }

    bitmap_ = CreateCompatibleBitmap(window_, rect_.right, rect_.bottom);
    if (!bitmap_) {
      ICE_TRACE_FORMAT("CreateCompatibleBitmap: 0");
      return ice::errc::not_available;
    }
    SelectObject(memory_, bitmap_);

    nk_init_default(&context_, nullptr);
    context_.clip.copy = copy;
    context_.clip.paste = paste;
    ice::ui::context::set(&context_);
    return {};
  }

  virtual void render() noexcept = 0;

  LRESULT handle(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
  {
    auto se = ice::on_scope_exit([hwnd]() {
      InvalidateRect(hwnd, nullptr, FALSE);
    });
    switch (msg) {
    case WM_SIZE: {
      const auto width = static_cast<LONG>(LOWORD(lparam));
      const auto height = static_cast<LONG>(HIWORD(lparam));
      if (rect_.right != width || rect_.bottom != height) {
        rect_.right = width;
        rect_.bottom = height;
        DeleteObject(bitmap_);
        bitmap_ = CreateCompatibleBitmap(window_, width, height);
        SelectObject(memory_, bitmap_);
      }
      return 0;
    }
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP: {
      const auto down = !((lparam >> 31) & 1);
      const auto ctrl = GetKeyState(VK_CONTROL) & (1 << 15);
      switch (wparam) {
      case VK_SHIFT:
      case VK_LSHIFT:
      case VK_RSHIFT:
        nk_input_key(&context_, NK_KEY_SHIFT, down);
        return 0;
      case VK_DELETE:
        nk_input_key(&context_, NK_KEY_DEL, down);
        return 0;
      case VK_RETURN:
        nk_input_key(&context_, NK_KEY_ENTER, down);
        return 0;
      case VK_TAB:
        nk_input_key(&context_, NK_KEY_TAB, down);
        return 0;
      case VK_LEFT:
        if (ctrl) {
          nk_input_key(&context_, NK_KEY_TEXT_WORD_LEFT, down);
        } else {
          nk_input_key(&context_, NK_KEY_LEFT, down);
        }
        return 0;
      case VK_RIGHT:
        if (ctrl) {
          nk_input_key(&context_, NK_KEY_TEXT_WORD_RIGHT, down);
        } else {
          nk_input_key(&context_, NK_KEY_RIGHT, down);
        }
        return 0;
      case VK_BACK:
        nk_input_key(&context_, NK_KEY_BACKSPACE, down);
        return 0;
      case VK_HOME:
        nk_input_key(&context_, NK_KEY_TEXT_START, down);
        nk_input_key(&context_, NK_KEY_SCROLL_START, down);
        return 0;
      case VK_END:
        nk_input_key(&context_, NK_KEY_TEXT_END, down);
        nk_input_key(&context_, NK_KEY_SCROLL_END, down);
        return 0;
      case VK_NEXT:
        nk_input_key(&context_, NK_KEY_SCROLL_DOWN, down);
        return 0;
      case VK_PRIOR:
        nk_input_key(&context_, NK_KEY_SCROLL_UP, down);
        return 0;
      case 'C':
        if (ctrl) {
          nk_input_key(&context_, NK_KEY_COPY, down);
          return 0;
        }
        break;
      case 'V':
        if (ctrl) {
          nk_input_key(&context_, NK_KEY_PASTE, down);
          return 0;
        }
        break;
      case 'X':
        if (ctrl) {
          nk_input_key(&context_, NK_KEY_CUT, down);
          return 0;
        }
        break;
      case 'Z':
        if (ctrl) {
          nk_input_key(&context_, NK_KEY_TEXT_UNDO, down);
          return 0;
        }
        break;
      case 'R':
        if (ctrl) {
          nk_input_key(&context_, NK_KEY_TEXT_REDO, down);
          return 0;
        }
        break;
      }
      break;
    }
    case WM_CHAR:
      if (wparam >= 32) {
        nk_input_unicode(&context_, static_cast<nk_rune>(wparam));
        return 0;
      }
      break;
    case WM_LBUTTONDOWN:
      nk_input_button(&context_, NK_BUTTON_LEFT, (short)LOWORD(lparam), (short)HIWORD(lparam), 1);
      SetCapture(hwnd);
      return 0;
    case WM_LBUTTONUP:
      nk_input_button(&context_, NK_BUTTON_DOUBLE, (short)LOWORD(lparam), (short)HIWORD(lparam), 0);
      nk_input_button(&context_, NK_BUTTON_LEFT, (short)LOWORD(lparam), (short)HIWORD(lparam), 0);
      ReleaseCapture();
      return 0;
    case WM_RBUTTONDOWN:
      nk_input_button(&context_, NK_BUTTON_RIGHT, (short)LOWORD(lparam), (short)HIWORD(lparam), 1);
      SetCapture(hwnd);
      return 0;
    case WM_RBUTTONUP:
      nk_input_button(&context_, NK_BUTTON_RIGHT, (short)LOWORD(lparam), (short)HIWORD(lparam), 0);
      ReleaseCapture();
      return 0;
    case WM_MBUTTONDOWN:
      nk_input_button(&context_, NK_BUTTON_MIDDLE, (short)LOWORD(lparam), (short)HIWORD(lparam), 1);
      SetCapture(hwnd);
      return 0;
    case WM_MBUTTONUP:
      nk_input_button(&context_, NK_BUTTON_MIDDLE, (short)LOWORD(lparam), (short)HIWORD(lparam), 0);
      ReleaseCapture();
      return 0;
    case WM_MOUSEWHEEL:
      nk_input_scroll(&context_, nk_vec2(0, (float)(short)HIWORD(wparam) / WHEEL_DELTA));
      return 0;
    case WM_MOUSEMOVE:
      nk_input_motion(&context_, (short)LOWORD(lparam), (short)HIWORD(lparam));
      return 0;
    case WM_LBUTTONDBLCLK:
      nk_input_button(&context_, NK_BUTTON_DOUBLE, (short)LOWORD(lparam), (short)HIWORD(lparam), 1);
      return 0;
    }
    se.cancel();
    return DefWindowProcA(hwnd, msg, wparam, lparam);
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
    return static_cast<float>(rect_.right);
  }

  constexpr float cy() const noexcept override
  {
    return static_cast<float>(rect_.bottom);
  }

  constexpr nk_context* get() noexcept
  {
    return &context_;
  }

  RECT* rect() noexcept
  {
    return &rect_;
  }

  constexpr HDC memory() const noexcept
  {
    return memory_;
  }

  constexpr HDC window() const noexcept
  {
    return window_;
  }

private:
  HWND hwnd_{};
  RECT rect_{};

  HDC window_{};
  HDC memory_{};
  int memory_state_{};

  HBITMAP bitmap_{};

  nk_context context_{};

  static void copy(nk_handle user, const char* string, int length) noexcept
  {}

  static void paste(nk_handle user, nk_text_edit* edit) noexcept
  {}
};

}  // namespace ice::os::windows
