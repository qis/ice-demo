#pragma once
#include "context.hpp"
#include <ice/format.hpp>
#include <ice/os/nuklear.hpp>
#include <ice/ui/font.hpp>
#include <windows.h>
#include <array>

namespace ice::os::windows {

class gdi final : public ice::os::windows::context {
public:
  class font final : public ice::ui::font {
  public:
    ~font() override
    {
      ice::ui::font::set(nullptr);
      if (font_.userdata.ptr) {
        font_.userdata = nk_handle_ptr(nullptr);
      }
      if (dc_) {
        RestoreDC(dc_, dc_state_);
      }
      if (handle_) {
        DeleteObject(handle_);
      }
      if (dc_) {
        DeleteDC(dc_);
      }
    }

    ice::error create(HDC dc, const std::string& name, int size, int weight, ice::ui::font::flags flags) noexcept
    {
      ICE_ASSERT(!dc_);
      ICE_ASSERT(!handle_);

      dc_ = CreateCompatibleDC(dc);
      if (!dc_) {
        ICE_TRACE_FORMAT("CreateCompatibleDC: NULL ({}, {}, {}, {})", name, size, weight, static_cast<int>(flags));
        return ice::errc::not_available;
      }

      dc_state_ = SaveDC(dc_);
      if (!dc_state_) {
        ICE_TRACE_FORMAT("SaveDC: 0 ({}, {}, {}, {})", name, size, weight, static_cast<int>(flags));
        return ice::errc::not_available;
      }

      const DWORD italic = flags & ice::ui::font::flags::italic;
      const DWORD underline = flags & ice::ui::font::flags::underline;
      const DWORD strikeout = flags & ice::ui::font::flags::strikeout;
      const int height = -MulDiv(size, GetDeviceCaps(dc, LOGPIXELSY), 72);

      handle_ = CreateFontA(height, 0, 0, 0, weight, italic, underline, strikeout, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, name.data());

      if (!handle_) {
        ICE_TRACE_FORMAT("CreateCompatibleDC: NULL ({}, {}, {}, {})", name, size, weight, static_cast<int>(flags));
        return ice::errc::not_available;
      }
      SelectObject(dc_, handle_);

      TEXTMETRIC metric;
      if (!GetTextMetricsA(dc_, &metric)) {
        ICE_TRACE_FORMAT("GetTextMetricsA: FALSE ({}, {}, {}, {})", name, size, weight, static_cast<int>(flags));
        return ice::errc::not_available;
      }
      font_.userdata = nk_handle_ptr(this);
      font_.height = static_cast<float>(metric.tmHeight);
      font_.width = get_text_width;
      ice::ui::font::set(&font_);
      return {};
    }

    float width(std::string_view text, float height) const noexcept override
    {
      const auto wlength = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
      const auto wstring = static_cast<WCHAR*>(_alloca(wlength * sizeof(wchar_t)));
      MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), wstring, wlength);
      SIZE size;
      if (GetTextExtentPoint32W(dc_, wstring, wlength, &size)) {
        return static_cast<float>(size.cx);
      }
      return -1.0f;
    }

    void render(HDC dc, const nk_command_text& c) const noexcept
    {
      const auto wlength = MultiByteToWideChar(CP_UTF8, 0, c.string, c.length, nullptr, 0);
      const auto wstring = static_cast<WCHAR*>(_alloca(wlength * sizeof(wchar_t)));
      MultiByteToWideChar(CP_UTF8, 0, c.string, c.length, wstring, wlength);
      SetBkColor(dc, get_native_color(c.background));
      SetTextColor(dc, get_native_color(c.foreground));
      SelectObject(dc, handle_);
      ExtTextOutW(dc, c.x, c.y, ETO_OPAQUE, nullptr, wstring, static_cast<UINT>(wlength), nullptr);
    }

    constexpr HDC dc() const noexcept
    {
      return dc_;
    }

  private:
    static float get_text_width(nk_handle handle, float height, const char* string, int length) noexcept
    {
      if (!handle.ptr || !string || length < 1) {
        return 0.0f;
      }
      return static_cast<const font*>(handle.ptr)->width({ string, static_cast<std::size_t>(length) }, height);
    }

    HDC dc_{};
    int dc_state_{};
    HFONT handle_{};
    nk_user_font font_{};
  };

  ice::error create(HWND hwnd, std::string_view name, int size, int weight, ice::ui::font::flags flags) noexcept
  {
    if (auto e = ice::os::windows::context::create(hwnd)) {
      ICE_TRACE_FUNCTION;
      return e;
    }
    std::string family{ name };
    if (family.empty()) {
      family = "Segoe UI";
    }
    if (auto e = font_.create(window(), family, size, weight, flags)) {
      ICE_TRACE_FUNCTION;
      return e;
    }
    nk_style_set_font(get(), font_.get());
    return {};
  }

  std::shared_ptr<ice::ui::font> create_font(std::string_view name, int size, int weight = 400,
    ice::ui::font::flags flags = ice::ui::font::flags::normal) noexcept override
  {
    auto font = std::make_shared<ice::os::windows::gdi::font>();
    if (auto e = font->create(window(), std::string{ name }, size, weight, flags)) {
      ICE_TRACE_FORMAT("Could not create GDI font: {}", e);
    }
    return font;
  }

  void render() noexcept override
  {
    HDC dc = memory();
    const RECT* rc = rect();
    SetBkColor(dc, 0x1E1E1E);
    SelectObject(dc, GetStockObject(DC_PEN));
    SelectObject(dc, GetStockObject(DC_BRUSH));
    ExtTextOutA(dc, 0, 0, ETO_OPAQUE, rc, nullptr, 0, nullptr);
    const nk_command* cmd = nullptr;
    nk_context* ctx = get();
    nk_foreach(cmd, ctx)
    {
      switch (cmd->type) {
      case NK_COMMAND_NOP:
        break;
      case NK_COMMAND_SCISSOR:
        render(dc, *reinterpret_cast<const nk_command_scissor*>(cmd));
        break;
      case NK_COMMAND_LINE:
        render(dc, *reinterpret_cast<const nk_command_line*>(cmd));
        break;
      case NK_COMMAND_RECT:
        render(dc, *reinterpret_cast<const nk_command_rect*>(cmd));
        break;
      case NK_COMMAND_RECT_FILLED:
        render(dc, *reinterpret_cast<const nk_command_rect_filled*>(cmd));
        break;
      case NK_COMMAND_CIRCLE:
        render(dc, *reinterpret_cast<const nk_command_circle*>(cmd));
        break;
      case NK_COMMAND_CIRCLE_FILLED:
        render(dc, *reinterpret_cast<const nk_command_circle_filled*>(cmd));
        break;
      case NK_COMMAND_TRIANGLE:
        render(dc, *reinterpret_cast<const nk_command_triangle*>(cmd));
        break;
      case NK_COMMAND_TRIANGLE_FILLED:
        render(dc, *reinterpret_cast<const nk_command_triangle_filled*>(cmd));
        break;
      case NK_COMMAND_POLYGON:
        render(dc, *reinterpret_cast<const nk_command_polygon*>(cmd));
        break;
      case NK_COMMAND_POLYGON_FILLED:
        render(dc, *reinterpret_cast<const nk_command_polygon_filled*>(cmd));
        break;
      case NK_COMMAND_POLYLINE:
        render(dc, *reinterpret_cast<const nk_command_polyline*>(cmd));
        break;
      case NK_COMMAND_TEXT:
        render(dc, *reinterpret_cast<const nk_command_text*>(cmd));
        break;
      case NK_COMMAND_CURVE:
        render(dc, *reinterpret_cast<const nk_command_curve*>(cmd));
        break;
      case NK_COMMAND_IMAGE:
        render(dc, *reinterpret_cast<const nk_command_image*>(cmd));
        break;
      case NK_COMMAND_ARC:
      case NK_COMMAND_ARC_FILLED:
      case NK_COMMAND_RECT_MULTI_COLOR:
      default:
        break;
      }
    }
    BitBlt(window(), 0, 0, rc->right, rc->bottom, dc, 0, 0, SRCCOPY);
  }

private:
  static void render(HDC dc, const nk_command_scissor& c) noexcept
  {
    const auto rc = get_native_rect(c);
    SelectClipRgn(dc, nullptr);
    IntersectClipRect(dc, rc.left, rc.top, rc.right + 1, rc.bottom + 1);
  }

  static void render(HDC dc, const nk_command_line& c) noexcept
  {
    HPEN pen = nullptr;
    const auto color = get_native_color(c.color);
    if (c.line_thickness == 1) {
      SetDCPenColor(dc, color);
    } else {
      pen = CreatePen(PS_SOLID, static_cast<int>(c.line_thickness), color);
      SelectObject(dc, pen);
    }
    MoveToEx(dc, c.begin.x, c.begin.y, nullptr);
    LineTo(dc, c.end.x, c.end.y);
    if (pen) {
      SelectObject(dc, GetStockObject(DC_PEN));
      DeleteObject(pen);
    }
  }

  static void render(HDC dc, const nk_command_rect& c) noexcept
  {
    HPEN pen = nullptr;
    const auto rc = get_native_rect(c);
    const auto color = get_native_color(c.color);
    if (c.line_thickness == 1) {
      SetDCPenColor(dc, color);
    } else {
      pen = CreatePen(PS_SOLID, static_cast<int>(c.line_thickness), color);
      SelectObject(dc, pen);
    }
    const auto brush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    if (!c.rounding) {
      Rectangle(dc, rc.left, rc.top, rc.right, rc.bottom);
    } else {
      const auto rounding = static_cast<int>(c.rounding);
      RoundRect(dc, rc.left, rc.top, rc.right, rc.bottom, rounding, rounding);
    }
    SelectObject(dc, brush);
    if (pen) {
      SelectObject(dc, GetStockObject(DC_PEN));
      DeleteObject(pen);
    }
  }

  static void render(HDC dc, const nk_command_rect_filled& c) noexcept
  {
    const auto rc = get_native_rect(c);
    const auto color = get_native_color(c.color);
    if (!c.rounding) {
      SetBkColor(dc, color);
      ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rc, nullptr, 0, nullptr);
    } else {
      SetDCPenColor(dc, color);
      SetDCBrushColor(dc, color);
      const auto rounding = static_cast<int>(c.rounding);
      RoundRect(dc, rc.left, rc.top, rc.right, rc.bottom, rounding, rounding);
    }
  }

  static void render(HDC dc, const nk_command_circle& c) noexcept
  {
    HPEN pen = nullptr;
    const auto rc = get_native_rect(c);
    const auto color = get_native_color(c.color);
    if (c.line_thickness == 1) {
      SetDCPenColor(dc, color);
    } else {
      pen = CreatePen(PS_SOLID, static_cast<int>(c.line_thickness), color);
      SelectObject(dc, pen);
    }
    SetDCBrushColor(dc, OPAQUE);
    Ellipse(dc, rc.left, rc.top, rc.right, rc.bottom);
    if (pen) {
      SelectObject(dc, GetStockObject(DC_PEN));
      DeleteObject(pen);
    }
  }

  static void render(HDC dc, const nk_command_circle_filled& c) noexcept
  {
    const auto rc = get_native_rect(c);
    const auto color = get_native_color(c.color);
    SetDCBrushColor(dc, color);
    SetDCPenColor(dc, color);
    Ellipse(dc, rc.left, rc.top, rc.right, rc.bottom);
  }

  static void render(HDC dc, const nk_command_triangle& c) noexcept
  {
    HPEN pen = nullptr;
    const auto color = get_native_color(c.color);
    if (c.line_thickness == 1) {
      SetDCPenColor(dc, color);
    } else {
      pen = CreatePen(PS_SOLID, static_cast<int>(c.line_thickness), color);
      SelectObject(dc, pen);
    }
    const POINT points[] = {
      { c.a.x, c.a.y },
      { c.b.x, c.b.y },
      { c.c.x, c.c.y },
      { c.a.x, c.a.y },
    };
    Polyline(dc, points, 4);
    if (pen) {
      SelectObject(dc, GetStockObject(DC_PEN));
      DeleteObject(pen);
    }
  }

  static void render(HDC dc, const nk_command_triangle_filled& c) noexcept
  {
    const auto color = get_native_color(c.color);
    SetDCPenColor(dc, color);
    SetDCBrushColor(dc, color);
    const POINT points[] = {
      { c.a.x, c.a.y },
      { c.b.x, c.b.y },
      { c.c.x, c.c.y },
    };
    Polygon(dc, points, 3);
  }

  static void render(HDC dc, const nk_command_polygon& c) noexcept
  {
    HPEN pen = nullptr;
    const auto color = get_native_color(c.color);
    if (c.line_thickness == 1) {
      SetDCPenColor(dc, color);
    } else {
      pen = CreatePen(PS_SOLID, static_cast<int>(c.line_thickness), color);
      SelectObject(dc, pen);
    }
    if (c.point_count) {
      MoveToEx(dc, c.points[0].x, c.points[0].y, nullptr);
      for (unsigned short i = 1; i < c.point_count; i++) {
        LineTo(dc, c.points[i].x, c.points[i].y);
      }
      LineTo(dc, c.points[0].x, c.points[0].y);
    }
    if (pen) {
      SelectObject(dc, GetStockObject(DC_PEN));
      DeleteObject(pen);
    }
  }

  static void render(HDC dc, const nk_command_polygon_filled& c) noexcept
  {
    const auto color = get_native_color(c.color);
    unsigned short i = 0;
    std::array<POINT, 64> points{};
    for (i = 0; i < c.point_count && i < points.size(); i++) {
      points[i].x = c.points[i].x;
      points[i].y = c.points[i].y;
    }
    SetDCBrushColor(dc, color);
    SetDCPenColor(dc, color);
    Polygon(dc, points.data(), i);
  }

  static void render(HDC dc, const nk_command_polyline& c) noexcept
  {
    HPEN pen = nullptr;
    const auto color = get_native_color(c.color);
    if (c.line_thickness == 1) {
      SetDCPenColor(dc, color);
    } else {
      pen = CreatePen(PS_SOLID, static_cast<int>(c.line_thickness), color);
      SelectObject(dc, pen);
    }
    if (c.point_count) {
      MoveToEx(dc, c.points[0].x, c.points[0].y, nullptr);
      for (unsigned short i = 1; i < c.point_count; i++) {
        LineTo(dc, c.points[i].x, c.points[i].y);
      }
    }
    if (pen) {
      SelectObject(dc, GetStockObject(DC_PEN));
      DeleteObject(pen);
    }
  }

  static void render(HDC dc, const nk_command_text& c) noexcept
  {
    if (c.length < 1 || !c.font || !c.font->userdata.ptr) {
      return;
    }
    return static_cast<const font*>(c.font->userdata.ptr)->render(dc, c);
  }

  static void render(HDC dc, const nk_command_curve& c) noexcept
  {
    HPEN pen = nullptr;
    const auto color = get_native_color(c.color);
    if (c.line_thickness == 1) {
      SetDCPenColor(dc, color);
    } else {
      pen = CreatePen(PS_SOLID, static_cast<int>(c.line_thickness), color);
      SelectObject(dc, pen);
    }
    SetDCBrushColor(dc, OPAQUE);
    const POINT points[] = {
      { c.begin.x, c.begin.y },
      { c.ctrl[0].x, c.ctrl[0].y },
      { c.ctrl[1].x, c.ctrl[1].y },
      { c.end.x, c.end.y },
    };
    PolyBezier(dc, points, 4);
    if (pen) {
      SelectObject(dc, GetStockObject(DC_PEN));
      DeleteObject(pen);
    }
  }

  static void render(HDC dc, const nk_command_image& c) noexcept
  {
    if (!c.img.handle.ptr) {
      return;
    }
    BITMAP bitmap{};
    const auto w = static_cast<int>(c.w);
    const auto h = static_cast<int>(c.h);
    const auto bitmap_dc = CreateCompatibleDC(dc);
    GetObject(static_cast<HBITMAP>(c.img.handle.ptr), sizeof(BITMAP), &bitmap);
    SelectObject(bitmap_dc, static_cast<HBITMAP>(c.img.handle.ptr));
    StretchBlt(dc, c.x, c.y, w, h, bitmap_dc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
    DeleteDC(bitmap_dc);
  }

  template <typename T>
  static inline RECT get_native_rect(const T& c) noexcept
  {
    return {
      static_cast<LONG>(c.x),
      static_cast<LONG>(c.y),
      static_cast<LONG>(c.x + c.w),
      static_cast<LONG>(c.y + c.h),
    };
  }

  static constexpr COLORREF get_native_color(nk_color color) noexcept
  {
    return color.r | (color.g << 8) | (color.b << 16);
  }

  font font_;
};

}  // namespace ice::os::windows
