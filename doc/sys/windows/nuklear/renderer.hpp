#pragma once
#include <ice/format.hpp>
#include <ice/nuklear.hpp>
#include <ice/sys/windows/config.hpp>
#include <windows.h>
#include <shellapi.h>
#include <array>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#include <nuklear.h>

namespace ice::sys::windows::nuklear {

struct renderer : public ice::nuklear {
  HWND hwnd{ nullptr };
  HDC window{ nullptr };
  HDC memory{ nullptr };
  HBITMAP memory_bitmap{ nullptr };

  RECT rc{ 0, 0, 1, 1 };

  nk_context context{};
  bool context_initialized{ false };

  ~renderer() override
  {
    if (context_initialized) {
      nk_free(&context);
    }
    if (memory) {
      DeleteDC(memory);
      memory = nullptr;
    }
    if (memory_bitmap) {
      DeleteObject(memory_bitmap);
      memory_bitmap = nullptr;
    }
    if (window) {
      ReleaseDC(hwnd, window);
      window = nullptr;
    }
  }

  void create(HWND hwnd) noexcept
  {
    // Get window size.
    GetClientRect(hwnd, &rc);

    // Get window device context.
    this->hwnd = hwnd;
    window = GetDC(hwnd);

    // Create memory device context.
    memory_bitmap = CreateCompatibleBitmap(window, rc.right, rc.bottom);
    memory = CreateCompatibleDC(window);
    SelectObject(memory, memory_bitmap);

    // Initialize nuklear.
    nk_init_default(&context, nullptr);
    context.clip.copy = clipboard_copy;
    context.clip.paste = clipboard_paste;
    context_initialized = true;
  }

  virtual void end(ice::nuklear::color color) noexcept = 0;

  void resize(LONG width, LONG height) noexcept
  {
    if (rc.right != width || rc.bottom != height) {
      rc.right = width;
      rc.bottom = height;
      DeleteObject(memory_bitmap);
      memory_bitmap = CreateCompatibleBitmap(window, width, height);
      SelectObject(memory, memory_bitmap);
    }
  }

  bool handle(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
  {
    switch (msg) {
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
        nk_input_key(&context, NK_KEY_SHIFT, down);
        return true;
      case VK_DELETE:
        nk_input_key(&context, NK_KEY_DEL, down);
        return true;
      case VK_RETURN:
        nk_input_key(&context, NK_KEY_ENTER, down);
        return true;
      case VK_TAB:
        nk_input_key(&context, NK_KEY_TAB, down);
        return true;
      case VK_LEFT:
        if (ctrl) {
          nk_input_key(&context, NK_KEY_TEXT_WORD_LEFT, down);
        } else {
          nk_input_key(&context, NK_KEY_LEFT, down);
        }
        return true;
      case VK_RIGHT:
        if (ctrl) {
          nk_input_key(&context, NK_KEY_TEXT_WORD_RIGHT, down);
        } else {
          nk_input_key(&context, NK_KEY_RIGHT, down);
        }
        return true;
      case VK_BACK:
        nk_input_key(&context, NK_KEY_BACKSPACE, down);
        return true;
      case VK_HOME:
        nk_input_key(&context, NK_KEY_TEXT_START, down);
        nk_input_key(&context, NK_KEY_SCROLL_START, down);
        return true;
      case VK_END:
        nk_input_key(&context, NK_KEY_TEXT_END, down);
        nk_input_key(&context, NK_KEY_SCROLL_END, down);
        return true;
      case VK_NEXT:
        nk_input_key(&context, NK_KEY_SCROLL_DOWN, down);
        return true;
      case VK_PRIOR:
        nk_input_key(&context, NK_KEY_SCROLL_UP, down);
        return true;
      case 'C':
        if (ctrl) {
          nk_input_key(&context, NK_KEY_COPY, down);
          return true;
        }
        break;
      case 'V':
        if (ctrl) {
          nk_input_key(&context, NK_KEY_PASTE, down);
          return true;
        }
        break;
      case 'X':
        if (ctrl) {
          nk_input_key(&context, NK_KEY_CUT, down);
          return true;
        }
        break;
      case 'Z':
        if (ctrl) {
          nk_input_key(&context, NK_KEY_TEXT_UNDO, down);
          return true;
        }
        break;
      case 'R':
        if (ctrl) {
          nk_input_key(&context, NK_KEY_TEXT_REDO, down);
          return true;
        }
        break;
      }
      break;
    }
    case WM_CHAR:
      if (wparam >= 32) {
        nk_input_unicode(&context, static_cast<nk_rune>(wparam));
        return true;
      }
      break;
    case WM_LBUTTONDOWN:
      nk_input_button(&context, NK_BUTTON_LEFT, (short)LOWORD(lparam), (short)HIWORD(lparam), 1);
      SetCapture(hwnd);
      return true;
    case WM_LBUTTONUP:
      nk_input_button(&context, NK_BUTTON_DOUBLE, (short)LOWORD(lparam), (short)HIWORD(lparam), 0);
      nk_input_button(&context, NK_BUTTON_LEFT, (short)LOWORD(lparam), (short)HIWORD(lparam), 0);
      ReleaseCapture();
      return true;
    case WM_RBUTTONDOWN:
      nk_input_button(&context, NK_BUTTON_RIGHT, (short)LOWORD(lparam), (short)HIWORD(lparam), 1);
      SetCapture(hwnd);
      return true;
    case WM_RBUTTONUP:
      nk_input_button(&context, NK_BUTTON_RIGHT, (short)LOWORD(lparam), (short)HIWORD(lparam), 0);
      ReleaseCapture();
      return true;
    case WM_MBUTTONDOWN:
      nk_input_button(&context, NK_BUTTON_MIDDLE, (short)LOWORD(lparam), (short)HIWORD(lparam), 1);
      SetCapture(hwnd);
      return true;
    case WM_MBUTTONUP:
      nk_input_button(&context, NK_BUTTON_MIDDLE, (short)LOWORD(lparam), (short)HIWORD(lparam), 0);
      ReleaseCapture();
      return true;
    case WM_MOUSEWHEEL:
      nk_input_scroll(&context, nk_vec2(0, (float)(short)HIWORD(wparam) / WHEEL_DELTA));
      return true;
    case WM_MOUSEMOVE:
      nk_input_motion(&context, (short)LOWORD(lparam), (short)HIWORD(lparam));
      return true;
    case WM_LBUTTONDBLCLK:
      nk_input_button(&context, NK_BUTTON_DOUBLE, (short)LOWORD(lparam), (short)HIWORD(lparam), 1);
      return true;
    }
    return false;
  }

  constexpr void input_begin() noexcept
  {
    nk_input_begin(&context);
  }

  constexpr void input_end() noexcept
  {
    nk_input_end(&context);
  }

  constexpr bool begin(const char* title, struct nk_rect bounds, nk_flags flags) noexcept
  {
    return nk_begin(&context, title, bounds, flags);
  }

  void layout_row_dynamic(float height, int cols) noexcept override
  {
    return nk_layout_row_dynamic(&context, height, cols);
  }

  void layout_row_static(float height, int item_width, int cols) noexcept override
  {
    nk_layout_row_static(&context, height, item_width, cols);
  }

  bool button_label(const char* text) noexcept override
  {
    return nk_button_label(&context, text);
  }

  bool option_label(const char* text, bool active) noexcept override
  {
    return nk_option_label(&context, text, active);
  }

  void property_int(const char* text, int min, int* val, int max, int step, float inc_per_pixel) noexcept override
  {
    return nk_property_int(&context, text, min, val, max, step, inc_per_pixel);
  }

  constexpr float cx() const noexcept
  {
    return static_cast<float>(rc.right);
  }

  constexpr float cy() const noexcept
  {
    return static_cast<float>(rc.bottom);
  }

  static void clipboard_copy(nk_handle usr, const char* text, int length)
  {}

  static void clipboard_paste(nk_handle usr, nk_text_edit* edit)
  {}
};

}  // namespace ice::sys::windows::nuklear
