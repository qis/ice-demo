#pragma once
#include "config.hpp"
#include "nuklear/gdi.hpp"
#include "nuklear/gdiplus.hpp"
#include <ice/application.hpp>
#include <ice/sys/window.hpp>

namespace ice::sys::windows {
namespace {

struct window_class {
  std::unique_ptr<ice::sys::windows::gdiplus::library> library;
  bool atom_initialized{ false };
  ATOM atom{ 0 };

  ~window_class()
  {
    if (atom_initialized) {
      UnregisterClassA(reinterpret_cast<LPCSTR>(static_cast<uintptr_t>(atom)), ice::application::instance());
    }
  }

  ice::error create(WNDPROC proc, HICON icon) noexcept
  {
    library = std::make_unique<ice::sys::windows::gdiplus::library>();
    if (!library->create()) {
      library.reset();
    }
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = proc;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.hInstance = ice::application::instance();
    wc.hIconSm = icon;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = "ice::window";
    atom = RegisterClassExA(&wc);
    if (!atom) {
      return get_last_error();
    }
    atom_initialized = true;
    return {};
  }

  static inline std::mutex mutex;
  static inline std::weak_ptr<window_class> pointer;
};

}  // namespace

class window final : public ice::sys::window, public std::enable_shared_from_this<window> {
public:
  void set(ice::window* window) noexcept override
  {
    window_.store(window, std::memory_order_release);
  }

  ice::error create(std::string_view text, std::string_view icon) noexcept override
  {
    window_class_ = window_class::pointer.lock();
    if (!window_class_) {
      std::lock_guard lock{ window_class::mutex };
      window_class_ = window_class::pointer.lock();
      if (!window_class_) {
        window_class_ = std::make_shared<window_class>();
        if (auto e = window_class_->create(proc, LoadIcon(ice::application::instance(), std::string{ icon }.data()))) {
          return e;
        }
        window_class::pointer = window_class_;
      }
    }

    const DWORD ws = WS_OVERLAPPEDWINDOW;
    const DWORD ex = WS_EX_APPWINDOW;

    RECT rc{ 0, 0, 640, 480 };
    AdjustWindowRectEx(&rc, ws, FALSE, ex);
    const auto x = CW_USEDEFAULT;
    const auto y = CW_USEDEFAULT;
    const auto cx = rc.right - rc.left;
    const auto cy = rc.bottom - rc.top;

    const auto name = reinterpret_cast<LPSTR>(window_class_->atom);
    const auto instance = ice::application::instance();
    if (!CreateWindowExA(ex, name, std::string{ text }.data(), ws, x, y, cx, cy, nullptr, nullptr, instance, this)) {
      return get_last_error();
    }
    system_ = shared_from_this();
    ICE_ASSERT(hwnd_);
    return {};
  }

  ice::error update(std::string_view text, std::string_view icon) noexcept override
  {
    return ice::errc::not_implemented;
  }

  ice::error destroy() noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    if (!PostMessageA(hwnd_, WM_CLOSE, 0, 0)) {
      return get_last_error();
    }
    return {};
  }

  ice::error show() noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    const DWORD flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_SHOWWINDOW;
    if (!SetWindowPos(hwnd_, nullptr, 0, 0, 0, 0, flags)) {
      return get_last_error();
    }
    return {};
  }

  ice::error hide() noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    const DWORD flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_HIDEWINDOW;
    if (!SetWindowPos(hwnd_, nullptr, 0, 0, 0, 0, flags)) {
      return get_last_error();
    }
    return {};
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

  ice::error set(ice::window::renderer renderer) noexcept override
  {
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    if (renderer_) {
      return ice::errc::not_initialized;
    }
    switch (renderer) {
    case ice::window::renderer::nuklear: {
      if (window_class_->library) {
        auto renderer = std::make_unique<nuklear::gdiplus>(*window_class_->library);
        if (const auto e = renderer->create(hwnd_); !e) {
          renderer->input_begin();
          renderer_ = std::move(renderer);
          return {};
        }
      }
      auto renderer = std::make_unique<nuklear::gdi>();
      if (auto e = renderer->create(hwnd_)) {
        return e;
      }
      renderer->input_begin();
      renderer_ = std::move(renderer);
      return {};
    } break;
    default:
      break;
    }
    return ice::errc::not_implemented;
  }

  ice::error run() noexcept override
  {
    MSG msg{};
    while (GetMessageA(&msg, hwnd_, 0, 0) > 0) {
      TranslateMessage(&msg);
      DispatchMessageA(&msg);
      while (PeekMessageA(&msg, hwnd_, 0, 0, PM_REMOVE) > 0) {
        if (msg.message == WM_QUIT) {
          system_.reset();
          return {};
        }
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
      }
    }
    system_.reset();
    return {};
  }

private:
  std::atomic<ice::window*> window_{};
  std::unique_ptr<nuklear::renderer> renderer_;
  std::shared_ptr<window_class> window_class_;
  std::shared_ptr<window> system_;
  HWND hwnd_{ nullptr };

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
      renderer_.reset();
      hwnd_ = nullptr;
      PostQuitMessage(EXIT_SUCCESS);
      SetWindowLongPtrA(hwnd, GWLP_USERDATA, 0);
      return 0;
    case WM_CLOSE:
      DestroyWindow(hwnd);
      return 0;
    case WM_SIZE: {
      if (renderer_) {
        const auto width = static_cast<LONG>(LOWORD(lparam));
        const auto height = static_cast<LONG>(HIWORD(lparam));
        renderer_->resize(width, height);
      }
      return 0;
    }
    case WM_PAINT:
      if (renderer_) {
        renderer_->input_end();
        ice::nuklear::color color;
        const auto cx = renderer_->cx();
        const auto cy = renderer_->cy();
        if (renderer_->begin("root", nk_rect(0.0f, 0.0f, cx, cy), NK_WINDOW_SCROLL_AUTO_HIDE)) {
          if (auto window = window_.load(std::memory_order_acquire)) {
            color = window->on_render(*renderer_, cx, cy);
          }
        }
        renderer_->end(color);
        renderer_->input_begin();
      }
      ValidateRect(hwnd, nullptr);
      return 0;
    case WM_ERASEBKGND:
      return 1;
    }
    if (renderer_ && renderer_->handle(hwnd, msg, wparam, lparam)) {
      InvalidateRect(hwnd, nullptr, FALSE);
      return 0;
    }
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }

  static LRESULT proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
  {
    if (const auto system = reinterpret_cast<window*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA))) {
      return system->handle(hwnd, msg, wparam, lparam);
    }
    if (msg == WM_CREATE) {
      const auto system = reinterpret_cast<window*>(reinterpret_cast<LPCREATESTRUCT>(lparam)->lpCreateParams);
      SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(system));
      return system->handle(hwnd, msg, wparam, lparam);
    }
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
};

}  // namespace ice::sys::windows
