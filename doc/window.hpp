#pragma once
#include "config.hpp"
#include <ice/application.hpp>
#include <ice/sys/window.hpp>

#ifdef ICE_TRACE_SYS_WINDOWS_WINDOW
#  include <map>
#  define ICE_TRACE_SYS_WINDOWS_WINDOW_FORMAT ICE_TRACE_FORMAT
#  define ICE_TRACE_SYS_WINDOWS_WINDOW_FUNCTION ICE_TRACE_FUNCTION
#else
#  define ICE_TRACE_SYS_WINDOWS_WINDOW_FORMAT(...)
#  define ICE_TRACE_SYS_WINDOWS_WINDOW_FUNCTION
#endif

namespace ice::sys::windows {
namespace {

struct window_class {
  ATOM atom{ 0 };

  ~window_class()
  {
    if (atom) {
      UnregisterClass(reinterpret_cast<LPCSTR>(static_cast<uintptr_t>(atom)), ice::application::instance());
    }
  }

  ice::error create(WNDPROC proc, HICON icon) noexcept
  {
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = proc;
    wc.hInstance = ice::application::instance();
    wc.hIconSm = icon;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = "ice::window";
    atom = RegisterClassEx(&wc);
    if (!atom) {
      return get_last_error();
    }
    return {};
  }

  static inline std::mutex mutex;
  static inline std::weak_ptr<window_class> pointer;
};

}  // namespace

#ifdef ICE_TRACE_SYS_WINDOWS_WINDOW
std::string message_name(DWORD message) noexcept;
#endif

class window final : public ice::sys::window, public std::enable_shared_from_this<window> {
public:
  void set(ice::window* window) noexcept override
  {
    ICE_TRACE_SYS_WINDOWS_WINDOW_FUNCTION;
    window_.store(window, std::memory_order_release);
  }

  ice::error create(std::string_view text, std::string_view icon) noexcept override
  {
    ICE_TRACE_SYS_WINDOWS_WINDOW_FORMAT("{}", text);
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
    const DWORD ex = WS_EX_NOREDIRECTIONBITMAP;
    const auto name = reinterpret_cast<LPSTR>(window_class_->atom);
    const auto instance = ice::application::instance();
    if (!CreateWindowExA(ex, name, std::string{ text }.data(), ws, 0, 0, 640, 480, nullptr, nullptr, instance, this)) {
      return get_last_error();
    }
    system_ = shared_from_this();
    ICE_ASSERT(hwnd_);
    return {};
  }

  ice::error update(std::string_view text, std::string_view icon) noexcept override
  {
    ICE_TRACE_SYS_WINDOWS_WINDOW_FUNCTION;
    return ice::errc::not_implemented;
  }

  ice::error destroy() noexcept override
  {
    ICE_TRACE_SYS_WINDOWS_WINDOW_FUNCTION;
    if (!hwnd_) {
      return ice::errc::not_initialized;
    }
    if (!PostMessage(hwnd_, WM_CLOSE, 0, 0)) {
      return get_last_error();
    }
    return {};
  }

  ice::error show() noexcept override
  {
    ICE_TRACE_SYS_WINDOWS_WINDOW_FUNCTION;
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
    ICE_TRACE_SYS_WINDOWS_WINDOW_FUNCTION;
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
    ICE_TRACE_SYS_WINDOWS_WINDOW_FUNCTION;
    return ice::errc::not_implemented;
  }

  ice::error maximize() noexcept override
  {
    ICE_TRACE_SYS_WINDOWS_WINDOW_FUNCTION;
    return ice::errc::not_implemented;
  }

  ice::error restore() noexcept override
  {
    ICE_TRACE_SYS_WINDOWS_WINDOW_FUNCTION;
    return ice::errc::not_implemented;
  }

  ice::error set(ice::window::mode mode) noexcept override
  {
    ICE_TRACE_SYS_WINDOWS_WINDOW_FORMAT("0x{:02X}", static_cast<unsigned>(mode));
    return ice::errc::not_implemented;
  }

  ice::error set(ice::window::style style) noexcept override
  {
    ICE_TRACE_SYS_WINDOWS_WINDOW_FORMAT("0x{:02X}", static_cast<unsigned>(style));
    return ice::errc::not_implemented;
  }

  ice::error run() noexcept override
  {
    MSG msg = {};
    while (GetMessage(&msg, hwnd_, 0, 0)) {
      ICE_TRACE_SYS_WINDOWS_WINDOW_FORMAT("{}", message_name(msg.message));
      DispatchMessage(&msg);
    }
    system_.reset();
    return {};
  }

private:
  LRESULT handle(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
  {
    ICE_TRACE_SYS_WINDOWS_WINDOW_FORMAT("{}", message_name(msg));
    switch (msg) {
    case WM_PAINT: {
      if (auto window = window_.load(std::memory_order_acquire)) {
        window->on_paint();
      }
      ValidateRect(hwnd_, nullptr);
      return 0;
    }
    case WM_CREATE: {
      hwnd_ = hwnd;
      hdc_ = GetDC(hwnd_);
      if (auto window = window_.load(std::memory_order_acquire)) {
        window->on_create();
      }
      return 0;
    }
    case WM_DESTROY: {
      if (auto window = window_.load(std::memory_order_acquire)) {
        window->on_destroy();
      }
      ReleaseDC(hwnd_, hdc_);
      hdc_ = nullptr;
      hwnd_ = nullptr;
      PostQuitMessage(EXIT_SUCCESS);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
      return 0;
    }
    case WM_CLOSE: {
      DestroyWindow(hwnd);
      return 0;
    }
    case WM_ERASEBKGND: return 1;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }

  static LRESULT proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
  {
    if (const auto system = reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))) {
      return system->handle(hwnd, msg, wparam, lparam);
    }
    if (msg == WM_CREATE) {
      const auto system = reinterpret_cast<window*>(reinterpret_cast<LPCREATESTRUCT>(lparam)->lpCreateParams);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(system));
      return system->handle(hwnd, msg, wparam, lparam);
    }
    ICE_TRACE_SYS_WINDOWS_WINDOW_FORMAT("{}", message_name(msg));
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }

  std::atomic<ice::window*> window_{};
  std::shared_ptr<window_class> window_class_{};
  std::shared_ptr<window> system_{};
  HWND hwnd_{};
  HDC hdc_{};


#if 0
  ice::error create(ice::window* target, ice::window::style style) noexcept override
  {
    target_.store(target, std::memory_order_release);

    if (auto window_class = window_class::get(proc); !window_class) {
      return window_class.error();
    } else {
      window_class_ = std::move(window_class.value());
    }

    // https://github.com/melak47/BorderlessWindow/blob/master/BorderlessWindow/src/BorderlessWindow.cpp
    // WS_OVERLAPPEDWINDOW  | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX

    const auto ws = WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
    const auto ex = WS_EX_NOREDIRECTIONBITMAP;
    const auto name = reinterpret_cast<LPCSTR>(window_class_->atom);
    const auto instance = window_class_->instance;
    const auto hwnd = CreateWindowEx(ex, name, "", ws, 0, 0, 640, 480, nullptr, nullptr, instance, this);
    if (!hwnd) {
      return get_last_error();
    }
    ICE_ASSERT(hwnd_);
    window_ = shared_from_this();
    return {};
  }

  ice::error rename(std::string_view text) noexcept override
  {
    if (!SetWindowText(hwnd_, std::string{ text }.data())) {
      return get_last_error();
    }
    return {};
  }

  ice::error destroy() noexcept override
  {
    if (tid_ == GetCurrentThreadId()) {
      if (!DestroyWindow(hwnd_)) {
        return get_last_error();
      }
    } else {
      if (!PostMessage(hwnd_, WM_ICE_WINDOW_DESTROY, 0, 0)) {
        return get_last_error();
      }
    }
    return {};
  }

  ice::error close() noexcept override
  {
    if (!PostMessage(hwnd_, WM_CLOSE, 0, 0)) {
      return get_last_error();
    }
    return {};
  }

  ice::error show() noexcept override
  {
    if (!ShowWindow(hwnd_, SW_SHOW)) {
      return get_last_error();
    }
    return {};
  }

  ice::error hide() noexcept override
  {
    if (!ShowWindow(hwnd_, SW_HIDE)) {
      return get_last_error();
    }
    return {};
  }

  int run() noexcept override
  {
    return EXIT_SUCCESS;
  }

  void set(ice::window* target) noexcept override
  {
    target_.store(target, std::memory_order_release);
  }

  LRESULT handle(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
  {
    switch (msg) {
    case WM_CREATE: {
      hwnd_ = hwnd;
      if (auto target = target_.load(std::memory_order_acquire)) {
        target->on_create();
      } else {
        show();
      }
      return 0;
    }
    case WM_CLOSE: {
      if (auto target = target_.load(std::memory_order_acquire)) {
        target->on_close();
      } else {
        hide();
        destroy();
      }
      return 0;
    }
    case WM_ICE_WINDOW_DESTROY: {
      DestroyWindow(hwnd);
      return 0;
    }
    case WM_DESTROY: {
      if (auto target = target_.load(std::memory_order_acquire)) {
        target->on_destroy();
      }
      target_.store(nullptr, std::memory_order_release);
      hwnd_ = nullptr;
      window_.reset();
      return 0;
    }
    case WM_ERASEBKGND: {
      return 1;
    }
    case WM_PAINT: {
      return 0;
    }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }

  static LRESULT proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
  {
    auto self = reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (msg == WM_CREATE) {
      self = reinterpret_cast<window*>(reinterpret_cast<LPCREATESTRUCT>(lparam)->lpCreateParams);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    } else if (msg == WM_DESTROY) {
      SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
    }
    return self ? self->handle(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
  }

private:
  std::atomic<ice::window*> target_{};
  std::shared_ptr<ice::sys::window> window_{};
  std::shared_ptr<window_class> window_class_{};
  HWND hwnd_{ nullptr };
  DWORD tid_{ 0 };
#endif
};

#ifdef ICE_TRACE_SYS_WINDOWS_WINDOW
inline std::string message_name(DWORD message) noexcept
{
  switch (message) {
  case WM_NULL: return "WM_NULL";
  case WM_CREATE: return "WM_CREATE";
  case WM_DESTROY: return "WM_DESTROY";
  case WM_MOVE: return "WM_MOVE";
  case WM_SIZE: return "WM_SIZE";
  case WM_ACTIVATE: return "WM_ACTIVATE";
  case WM_SETFOCUS: return "WM_SETFOCUS";
  case WM_KILLFOCUS: return "WM_KILLFOCUS";
  case WM_ENABLE: return "WM_ENABLE";
  case WM_SETREDRAW: return "WM_SETREDRAW";
  case WM_SETTEXT: return "WM_SETTEXT";
  case WM_GETTEXT: return "WM_GETTEXT";
  case WM_GETTEXTLENGTH: return "WM_GETTEXTLENGTH";
  case WM_PAINT: return "WM_PAINT";
  case WM_CLOSE: return "WM_CLOSE";
  case WM_QUERYENDSESSION: return "WM_QUERYENDSESSION";
  case WM_QUERYOPEN: return "WM_QUERYOPEN";
  case WM_ENDSESSION: return "WM_ENDSESSION";
  case WM_QUIT: return "WM_QUIT";
  case WM_ERASEBKGND: return "WM_ERASEBKGND";
  case WM_SYSCOLORCHANGE: return "WM_SYSCOLORCHANGE";
  case WM_SHOWWINDOW: return "WM_SHOWWINDOW";
  case WM_WININICHANGE: return "WM_WININICHANGE";
  case WM_DEVMODECHANGE: return "WM_DEVMODECHANGE";
  case WM_ACTIVATEAPP: return "WM_ACTIVATEAPP";
  case WM_FONTCHANGE: return "WM_FONTCHANGE";
  case WM_TIMECHANGE: return "WM_TIMECHANGE";
  case WM_CANCELMODE: return "WM_CANCELMODE";
  case WM_SETCURSOR: return "WM_SETCURSOR";
  case WM_MOUSEACTIVATE: return "WM_MOUSEACTIVATE";
  case WM_CHILDACTIVATE: return "WM_CHILDACTIVATE";
  case WM_QUEUESYNC: return "WM_QUEUESYNC";
  case WM_GETMINMAXINFO: return "WM_GETMINMAXINFO";
  case WM_PAINTICON: return "WM_PAINTICON";
  case WM_ICONERASEBKGND: return "WM_ICONERASEBKGND";
  case WM_NEXTDLGCTL: return "WM_NEXTDLGCTL";
  case WM_SPOOLERSTATUS: return "WM_SPOOLERSTATUS";
  case WM_DRAWITEM: return "WM_DRAWITEM";
  case WM_MEASUREITEM: return "WM_MEASUREITEM";
  case WM_DELETEITEM: return "WM_DELETEITEM";
  case WM_VKEYTOITEM: return "WM_VKEYTOITEM";
  case WM_CHARTOITEM: return "WM_CHARTOITEM";
  case WM_SETFONT: return "WM_SETFONT";
  case WM_GETFONT: return "WM_GETFONT";
  case WM_SETHOTKEY: return "WM_SETHOTKEY";
  case WM_GETHOTKEY: return "WM_GETHOTKEY";
  case WM_QUERYDRAGICON: return "WM_QUERYDRAGICON";
  case WM_COMPAREITEM: return "WM_COMPAREITEM";
  case WM_GETOBJECT: return "WM_GETOBJECT";
  case WM_COMPACTING: return "WM_COMPACTING";
  case WM_COMMNOTIFY: return "WM_COMMNOTIFY";
  case WM_WINDOWPOSCHANGING: return "WM_WINDOWPOSCHANGING";
  case WM_WINDOWPOSCHANGED: return "WM_WINDOWPOSCHANGED";
  case WM_POWER: return "WM_POWER";
  case WM_COPYDATA: return "WM_COPYDATA";
  case WM_CANCELJOURNAL: return "WM_CANCELJOURNAL";
  case WM_NOTIFY: return "WM_NOTIFY";
  case WM_INPUTLANGCHANGEREQUEST: return "WM_INPUTLANGCHANGEREQUEST";
  case WM_INPUTLANGCHANGE: return "WM_INPUTLANGCHANGE";
  case WM_TCARD: return "WM_TCARD";
  case WM_HELP: return "WM_HELP";
  case WM_USERCHANGED: return "WM_USERCHANGED";
  case WM_NOTIFYFORMAT: return "WM_NOTIFYFORMAT";
  case WM_CONTEXTMENU: return "WM_CONTEXTMENU";
  case WM_STYLECHANGING: return "WM_STYLECHANGING";
  case WM_STYLECHANGED: return "WM_STYLECHANGED";
  case WM_DISPLAYCHANGE: return "WM_DISPLAYCHANGE";
  case WM_GETICON: return "WM_GETICON";
  case WM_SETICON: return "WM_SETICON";
  case WM_NCCREATE: return "WM_NCCREATE";
  case WM_NCDESTROY: return "WM_NCDESTROY";
  case WM_NCCALCSIZE: return "WM_NCCALCSIZE";
  case WM_NCHITTEST: return "WM_NCHITTEST";
  case WM_NCPAINT: return "WM_NCPAINT";
  case WM_NCACTIVATE: return "WM_NCACTIVATE";
  case WM_GETDLGCODE: return "WM_GETDLGCODE";
  case WM_SYNCPAINT: return "WM_SYNCPAINT";
  case WM_NCMOUSEMOVE: return "WM_NCMOUSEMOVE";
  case WM_NCLBUTTONDOWN: return "WM_NCLBUTTONDOWN";
  case WM_NCLBUTTONUP: return "WM_NCLBUTTONUP";
  case WM_NCLBUTTONDBLCLK: return "WM_NCLBUTTONDBLCLK";
  case WM_NCRBUTTONDOWN: return "WM_NCRBUTTONDOWN";
  case WM_NCRBUTTONUP: return "WM_NCRBUTTONUP";
  case WM_NCRBUTTONDBLCLK: return "WM_NCRBUTTONDBLCLK";
  case WM_NCMBUTTONDOWN: return "WM_NCMBUTTONDOWN";
  case WM_NCMBUTTONUP: return "WM_NCMBUTTONUP";
  case WM_NCMBUTTONDBLCLK: return "WM_NCMBUTTONDBLCLK";
  case WM_NCXBUTTONDOWN: return "WM_NCXBUTTONDOWN";
  case WM_NCXBUTTONUP: return "WM_NCXBUTTONUP";
  case WM_NCXBUTTONDBLCLK: return "WM_NCXBUTTONDBLCLK";
  case WM_INPUT_DEVICE_CHANGE: return "WM_INPUT_DEVICE_CHANGE";
  case WM_INPUT: return "WM_INPUT";
  case WM_KEYFIRST: return "WM_KEYDOWN|WM_KEYFIRST|WM_KEYLAST";
  case WM_KEYUP: return "WM_KEYUP";
  case WM_CHAR: return "WM_CHAR";
  case WM_DEADCHAR: return "WM_DEADCHAR";
  case WM_SYSKEYDOWN: return "WM_SYSKEYDOWN";
  case WM_SYSKEYUP: return "WM_SYSKEYUP";
  case WM_SYSCHAR: return "WM_SYSCHAR";
  case WM_SYSDEADCHAR: return "WM_SYSDEADCHAR";
  case WM_UNICHAR: return "WM_UNICHAR";
  case UNICODE_NOCHAR: return "UNICODE_NOCHAR";
  case WM_IME_STARTCOMPOSITION: return "WM_IME_STARTCOMPOSITION";
  case WM_IME_ENDCOMPOSITION: return "WM_IME_ENDCOMPOSITION";
  case WM_IME_COMPOSITION: return "WM_IME_COMPOSITION|WM_IME_KEYLAST";
  case WM_INITDIALOG: return "WM_INITDIALOG";
  case WM_COMMAND: return "WM_COMMAND";
  case WM_SYSCOMMAND: return "WM_SYSCOMMAND";
  case WM_TIMER: return "WM_TIMER";
  case WM_HSCROLL: return "WM_HSCROLL";
  case WM_VSCROLL: return "WM_VSCROLL";
  case WM_INITMENU: return "WM_INITMENU";
  case WM_INITMENUPOPUP: return "WM_INITMENUPOPUP";
  case WM_GESTURE: return "WM_GESTURE";
  case WM_GESTURENOTIFY: return "WM_GESTURENOTIFY";
  case WM_MENUSELECT: return "WM_MENUSELECT";
  case WM_MENUCHAR: return "WM_MENUCHAR";
  case WM_ENTERIDLE: return "WM_ENTERIDLE";
  case WM_MENURBUTTONUP: return "WM_MENURBUTTONUP";
  case WM_MENUDRAG: return "WM_MENUDRAG";
  case WM_MENUGETOBJECT: return "WM_MENUGETOBJECT";
  case WM_UNINITMENUPOPUP: return "WM_UNINITMENUPOPUP";
  case WM_MENUCOMMAND: return "WM_MENUCOMMAND";
  case WM_CHANGEUISTATE: return "WM_CHANGEUISTATE";
  case WM_UPDATEUISTATE: return "WM_UPDATEUISTATE";
  case WM_QUERYUISTATE: return "WM_QUERYUISTATE";
  case WM_CTLCOLORMSGBOX: return "WM_CTLCOLORMSGBOX";
  case WM_CTLCOLOREDIT: return "WM_CTLCOLOREDIT";
  case WM_CTLCOLORLISTBOX: return "WM_CTLCOLORLISTBOX";
  case WM_CTLCOLORBTN: return "WM_CTLCOLORBTN";
  case WM_CTLCOLORDLG: return "WM_CTLCOLORDLG";
  case WM_CTLCOLORSCROLLBAR: return "WM_CTLCOLORSCROLLBAR";
  case WM_CTLCOLORSTATIC: return "WM_CTLCOLORSTATIC";
  case MN_GETHMENU: return "MN_GETHMENU";
  case WM_MOUSEMOVE: return "WM_MOUSEMOVE|WM_MOUSEFIRST";
  case WM_LBUTTONDOWN: return "WM_LBUTTONDOWN";
  case WM_LBUTTONUP: return "WM_LBUTTONUP";
  case WM_LBUTTONDBLCLK: return "WM_LBUTTONDBLCLK";
  case WM_RBUTTONDOWN: return "WM_RBUTTONDOWN";
  case WM_RBUTTONUP: return "WM_RBUTTONUP";
  case WM_RBUTTONDBLCLK: return "WM_RBUTTONDBLCLK";
  case WM_MBUTTONDOWN: return "WM_MBUTTONDOWN";
  case WM_MBUTTONUP: return "WM_MBUTTONUP";
  case WM_MBUTTONDBLCLK: return "WM_MBUTTONDBLCLK";
  case WM_MOUSEWHEEL: return "WM_MOUSEWHEEL";
  case WM_XBUTTONDOWN: return "WM_XBUTTONDOWN";
  case WM_XBUTTONUP: return "WM_XBUTTONUP";
  case WM_XBUTTONDBLCLK: return "WM_XBUTTONDBLCLK";
  case WM_MOUSEHWHEEL: return "WM_MOUSEHWHEEL|WM_MOUSELAST";
  case WM_PARENTNOTIFY: return "WM_PARENTNOTIFY";
  case WM_ENTERMENULOOP: return "WM_ENTERMENULOOP";
  case WM_EXITMENULOOP: return "WM_EXITMENULOOP";
  case WM_NEXTMENU: return "WM_NEXTMENU";
  case WM_SIZING: return "WM_SIZING";
  case WM_CAPTURECHANGED: return "WM_CAPTURECHANGED";
  case WM_MOVING: return "WM_MOVING";
  case WM_POWERBROADCAST: return "WM_POWERBROADCAST";
  case WM_DEVICECHANGE: return "WM_DEVICECHANGE";
  case WM_MDICREATE: return "WM_MDICREATE";
  case WM_MDIDESTROY: return "WM_MDIDESTROY";
  case WM_MDIACTIVATE: return "WM_MDIACTIVATE";
  case WM_MDIRESTORE: return "WM_MDIRESTORE";
  case WM_MDINEXT: return "WM_MDINEXT";
  case WM_MDIMAXIMIZE: return "WM_MDIMAXIMIZE";
  case WM_MDITILE: return "WM_MDITILE";
  case WM_MDICASCADE: return "WM_MDICASCADE";
  case WM_MDIICONARRANGE: return "WM_MDIICONARRANGE";
  case WM_MDIGETACTIVE: return "WM_MDIGETACTIVE";
  case WM_MDISETMENU: return "WM_MDISETMENU";
  case WM_ENTERSIZEMOVE: return "WM_ENTERSIZEMOVE";
  case WM_EXITSIZEMOVE: return "WM_EXITSIZEMOVE";
  case WM_DROPFILES: return "WM_DROPFILES";
  case WM_MDIREFRESHMENU: return "WM_MDIREFRESHMENU";
  case WM_POINTERDEVICECHANGE: return "WM_POINTERDEVICECHANGE";
  case WM_POINTERDEVICEINRANGE: return "WM_POINTERDEVICEINRANGE";
  case WM_POINTERDEVICEOUTOFRANGE: return "WM_POINTERDEVICEOUTOFRANGE";
  case WM_TOUCH: return "WM_TOUCH";
  case WM_NCPOINTERUPDATE: return "WM_NCPOINTERUPDATE";
  case WM_NCPOINTERDOWN: return "WM_NCPOINTERDOWN";
  case WM_NCPOINTERUP: return "WM_NCPOINTERUP";
  case WM_POINTERUPDATE: return "WM_POINTERUPDATE";
  case WM_POINTERDOWN: return "WM_POINTERDOWN";
  case WM_POINTERUP: return "WM_POINTERUP";
  case WM_POINTERENTER: return "WM_POINTERENTER";
  case WM_POINTERLEAVE: return "WM_POINTERLEAVE";
  case WM_POINTERACTIVATE: return "WM_POINTERACTIVATE";
  case WM_POINTERCAPTURECHANGED: return "WM_POINTERCAPTURECHANGED";
  case WM_TOUCHHITTESTING: return "WM_TOUCHHITTESTING";
  case WM_POINTERWHEEL: return "WM_POINTERWHEEL";
  case WM_POINTERHWHEEL: return "WM_POINTERHWHEEL";
  case DM_POINTERHITTEST: return "DM_POINTERHITTEST";
  case WM_POINTERROUTEDTO: return "WM_POINTERROUTEDTO";
  case WM_POINTERROUTEDAWAY: return "WM_POINTERROUTEDAWAY";
  case WM_POINTERROUTEDRELEASED: return "WM_POINTERROUTEDRELEASED";
  case WM_IME_SETCONTEXT: return "WM_IME_SETCONTEXT";
  case WM_IME_NOTIFY: return "WM_IME_NOTIFY";
  case WM_IME_CONTROL: return "WM_IME_CONTROL";
  case WM_IME_COMPOSITIONFULL: return "WM_IME_COMPOSITIONFULL";
  case WM_IME_SELECT: return "WM_IME_SELECT";
  case WM_IME_CHAR: return "WM_IME_CHAR";
  case WM_IME_REQUEST: return "WM_IME_REQUEST";
  case WM_IME_KEYDOWN: return "WM_IME_KEYDOWN";
  case WM_IME_KEYUP: return "WM_IME_KEYUP";
  case WM_MOUSEHOVER: return "WM_MOUSEHOVER";
  case WM_MOUSELEAVE: return "WM_MOUSELEAVE";
  case WM_NCMOUSEHOVER: return "WM_NCMOUSEHOVER";
  case WM_NCMOUSELEAVE: return "WM_NCMOUSELEAVE";
  case WM_WTSSESSION_CHANGE: return "WM_WTSSESSION_CHANGE";
  case WM_TABLET_FIRST: return "WM_TABLET_FIRST";
  case WM_TABLET_LAST: return "WM_TABLET_LAST";
  case WM_DPICHANGED: return "WM_DPICHANGED";
  case WM_DPICHANGED_BEFOREPARENT: return "WM_DPICHANGED_BEFOREPARENT";
  case WM_DPICHANGED_AFTERPARENT: return "WM_DPICHANGED_AFTERPARENT";
  case WM_GETDPISCALEDSIZE: return "WM_GETDPISCALEDSIZE";
  case WM_CUT: return "WM_CUT";
  case WM_COPY: return "WM_COPY";
  case WM_PASTE: return "WM_PASTE";
  case WM_CLEAR: return "WM_CLEAR";
  case WM_UNDO: return "WM_UNDO";
  case WM_RENDERFORMAT: return "WM_RENDERFORMAT";
  case WM_RENDERALLFORMATS: return "WM_RENDERALLFORMATS";
  case WM_DESTROYCLIPBOARD: return "WM_DESTROYCLIPBOARD";
  case WM_DRAWCLIPBOARD: return "WM_DRAWCLIPBOARD";
  case WM_PAINTCLIPBOARD: return "WM_PAINTCLIPBOARD";
  case WM_VSCROLLCLIPBOARD: return "WM_VSCROLLCLIPBOARD";
  case WM_SIZECLIPBOARD: return "WM_SIZECLIPBOARD";
  case WM_ASKCBFORMATNAME: return "WM_ASKCBFORMATNAME";
  case WM_CHANGECBCHAIN: return "WM_CHANGECBCHAIN";
  case WM_HSCROLLCLIPBOARD: return "WM_HSCROLLCLIPBOARD";
  case WM_QUERYNEWPALETTE: return "WM_QUERYNEWPALETTE";
  case WM_PALETTEISCHANGING: return "WM_PALETTEISCHANGING";
  case WM_PALETTECHANGED: return "WM_PALETTECHANGED";
  case WM_HOTKEY: return "WM_HOTKEY";
  case WM_PRINT: return "WM_PRINT";
  case WM_PRINTCLIENT: return "WM_PRINTCLIENT";
  case WM_APPCOMMAND: return "WM_APPCOMMAND";
  case WM_THEMECHANGED: return "WM_THEMECHANGED";
  case WM_CLIPBOARDUPDATE: return "WM_CLIPBOARDUPDATE";
  case WM_DWMCOMPOSITIONCHANGED: return "WM_DWMCOMPOSITIONCHANGED";
  case WM_DWMNCRENDERINGCHANGED: return "WM_DWMNCRENDERINGCHANGED";
  case WM_DWMCOLORIZATIONCOLORCHANGED: return "WM_DWMCOLORIZATIONCOLORCHANGED";
  case WM_DWMWINDOWMAXIMIZEDCHANGE: return "WM_DWMWINDOWMAXIMIZEDCHANGE";
  case WM_DWMSENDICONICTHUMBNAIL: return "WM_DWMSENDICONICTHUMBNAIL";
  case WM_DWMSENDICONICLIVEPREVIEWBITMAP: return "WM_DWMSENDICONICLIVEPREVIEWBITMAP";
  case WM_GETTITLEBARINFOEX: return "WM_GETTITLEBARINFOEX";
  case WM_HANDHELDFIRST: return "WM_HANDHELDFIRST";
  case WM_HANDHELDLAST: return "WM_HANDHELDLAST";
  case WM_AFXFIRST: return "WM_AFXFIRST";
  case WM_AFXLAST: return "WM_AFXLAST";
  case WM_PENWINFIRST: return "WM_PENWINFIRST";
  case WM_PENWINLAST: return "WM_PENWINLAST";
  case WM_APP: return "WM_APP";
  case WM_USER: return "WM_USER";
  }
  return fmt::format("0x{:04X}", message);
}
#endif

}  // namespace ice::sys::windows
