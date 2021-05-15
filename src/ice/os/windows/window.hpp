#pragma once
#include "context.hpp"
#include <ice/application.hpp>
#include <ice/os/window.hpp>
#include <ice/os/windows/gdi.hpp>
#include <ice/os/windows/gdiplus.hpp>
#include <ice/utility.hpp>
#include <mutex>

#define ICE_WM_DESTROY (WM_APP + 1)

namespace ice::os::windows {

class window_class {
public:
  static inline std::mutex mutex;

  ~window_class()
  {
    if (brush_) {
      DeleteObject(brush_);
    }
    if (atom_) {
      UnregisterClassA(reinterpret_cast<LPCSTR>(static_cast<uintptr_t>(atom_)), ice::application::instance());
    }
  }

  ice::error create() noexcept
  {
    if (atom_) {
      return {};
    }
    std::lock_guard lock{ mutex };
    if (atom_) {
      return {};
    }
    if (!brush_) {
      brush_ = reinterpret_cast<HBRUSH>(CreateSolidBrush(0x1E1E1E));
    }
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = proc;
    wc.hbrBackground = brush_;
    wc.hInstance = ice::application::instance();
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = "ice::window";
    atom_ = RegisterClassExA(&wc);
    if (!atom_) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("RegisterClassExA: {}", e);
      return e;
    }
    return {};
  }

  constexpr ATOM atom() const noexcept
  {
    return atom_;
  }

  static LRESULT proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;

  static std::shared_ptr<window_class> get_shared_object() noexcept
  {
    static std::weak_ptr<window_class> wp;
    auto sp = wp.lock();
    if (!sp) {
      std::lock_guard lock{ mutex };
      sp = wp.lock();
      if (!sp) {
        sp = std::make_shared<window_class>();
        wp = sp;
      }
    }
    return sp;
  }

private:
  ATOM atom_{};
  HBRUSH brush_{};
};

class window final : public ice::os::window, public std::enable_shared_from_this<window> {
public:
  ~window() override
  {
    window_.store(nullptr, std::memory_order_release);
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
    if (hwnd_) {
      return {};
    }

    window_class_ = window_class::get_shared_object();
    if (auto e = window_class_->create()) {
      ICE_TRACE_FUNCTION;
      return e;
    }

    constexpr DWORD ws = WS_OVERLAPPEDWINDOW;
    constexpr DWORD ex = WS_EX_APPWINDOW;

    RECT rc{ 0, 0, 640, 480 };
    if (!AdjustWindowRectEx(&rc, ws, FALSE, ex)) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("AdjustWindowRectEx: {}", e);
      return e;
    }

    constexpr int x = CW_USEDEFAULT;
    constexpr int y = CW_USEDEFAULT;
    const int cx = rc.right - rc.left;
    const int cy = rc.bottom - rc.top;
    const auto name = reinterpret_cast<LPSTR>(window_class_->atom());
    const auto instance = ice::application::instance();
    if (!CreateWindowExA(ex, name, "", ws, x, y, cx, cy, nullptr, nullptr, instance, this)) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("CreateWindowExA: {}", e);
      return e;
    }
    self_ = shared_from_this();
    ICE_ASSERT(hwnd_);
    return {};
  }

  ice::error destroy() noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    if (!PostMessageA(hwnd_, ICE_WM_DESTROY, 0, 0)) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("PostMessageA: {}", e);
      return e;
    }
    return ice::errc::not_implemented;
  }

  ice::error set(ice::window::mode mode) noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error set(ice::window::style style) noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error text(std::string_view text) noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error icon(std::string_view icon) noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error start(std::string_view name, int size, int weight, ice::ui::font::flags flags) noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
#if ICE_OS_WINDOWS_GDIPLUS
    auto gdiplus = std::make_unique<ice::os::windows::gdiplus>();
    if (auto e = gdiplus->create(hwnd_, name, size, weight, flags)) {
      ICE_TRACE_FORMAT("Could not create GDI+ context: {}", e);
#endif
      auto gdi = std::make_unique<ice::os::windows::gdi>();
      if (auto e = gdi->create(hwnd_, name, size, weight, flags)) {
        ICE_TRACE_FORMAT("Could not create GDI context: {}", e);
        return e;
      }
      context_ = std::move(gdi);
      return {};
#if ICE_OS_WINDOWS_GDIPLUS
    }
    context_ = std::move(gdiplus);
    return {};
#endif
  }

  ice::error show() noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    constexpr DWORD flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_SHOWWINDOW;
    if (!SetWindowPos(hwnd_, nullptr, 0, 0, 0, 0, flags)) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("SetWindowPos: {}", e);
      return e;
    }
    return {};
  }

  ice::error hide() noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    constexpr DWORD flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_HIDEWINDOW;
    if (!SetWindowPos(hwnd_, nullptr, 0, 0, 0, 0, flags)) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("SetWindowPos: {}", e);
      return e;
    }
    return {};
  }

  ice::error minimize() noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error maximize() noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error restore() noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    return ice::errc::not_implemented;
  }

  ice::error run() noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    const auto se = ice::on_scope_exit([this]() {
      self_.reset();
    });
    MSG msg{};
    while (GetMessageA(&msg, hwnd_, 0, 0) > 0) {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
      while (PeekMessageA(&msg, hwnd_, 0, 0, PM_REMOVE) > 0) {
        if (msg.message == WM_QUIT) {
          return {};
        }
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
      }
    }
    return {};
  }

  LRESULT handle(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
  {
    switch (msg) {
    case WM_CREATE:
      hwnd_ = hwnd;
      if (auto window = window_.load(std::memory_order_acquire)) {
        window->on_create();
      }
      return 0;
    case WM_DESTROY:
      if (auto window = window_.load(std::memory_order_acquire)) {
        window->on_destroy();
      }
      context_.reset();
      hwnd_ = nullptr;
      PostQuitMessage(EXIT_SUCCESS);
      SetWindowLongPtrA(hwnd, GWLP_USERDATA, 0);
      return 0;
    case WM_CLOSE:
      if (auto window = window_.load(std::memory_order_acquire)) {
        window->on_close();
      } else {
        DestroyWindow(hwnd);
      }
      return 0;
    case WM_PAINT:
      if (context_) {
        context_->input_end();
        const auto cx = context_->cx();
        const auto cy = context_->cy();
        if (context_->begin("root", nk_rect(0.0f, 0.0f, cx, cy), NK_WINDOW_SCROLL_AUTO_HIDE)) {
          if (auto window = window_.load(std::memory_order_acquire)) {
            window->on_render(*context_);
          }
        }
        context_->end();
        context_->render();
        context_->clear();
        context_->input_begin();
      }
      ValidateRect(hwnd, nullptr);
      return 0;
    case WM_ERASEBKGND:
      return 1;
    case ICE_WM_DESTROY:
      DestroyWindow(hwnd);
      return 0;
    }
    return context_ ? context_->handle(hwnd, msg, wparam, lparam) : DefWindowProcA(hwnd, msg, wparam, lparam);
  }

private:
  std::shared_ptr<window> self_;
  std::atomic<ice::window*> window_;
  std::shared_ptr<window_class> window_class_;
  std::unique_ptr<ice::os::windows::context> context_;
  HWND hwnd_{};
};

inline LRESULT window_class::proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
{
  if (const auto window = reinterpret_cast<ice::os::windows::window*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA))) {
    return window->handle(hwnd, msg, wparam, lparam);
  }
  if (msg == WM_CREATE) {
    const auto window = reinterpret_cast<ice::os::windows::window*>(
      reinterpret_cast<LPCREATESTRUCT>(lparam)->lpCreateParams);
    SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    return window->handle(hwnd, msg, wparam, lparam);
  }
  return DefWindowProcA(hwnd, msg, wparam, lparam);
}

}  // namespace ice::os::windows
