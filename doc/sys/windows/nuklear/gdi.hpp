#pragma once
#include "renderer.hpp"
#include <ice/sys/windows/gdiplus.hpp>

namespace ice::sys::windows::nuklear {

class gdi final : public renderer {
public:
  ~gdi() override
  {
    if (font_.userdata.ptr) {
      font_.userdata = nk_handle_ptr(nullptr);
    }
    if (font_data_.handle) {
      DeleteObject(font_data_.handle);
      font_data_.handle = nullptr;
    }
    if (font_data_.dc) {
      DeleteDC(font_data_.dc);
      font_data_.dc = nullptr;
    }
  }

  ice::error create(HWND hwnd) noexcept
  {
    renderer::create(hwnd);

    // Create font.
    font_data_.dc = CreateCompatibleDC(nullptr);
    font_data_.handle = CreateFontA(-MulDiv(9, GetDeviceCaps(window, LOGPIXELSY), 72),
      0,
      0,
      0,
      FW_NORMAL,
      FALSE,
      FALSE,
      FALSE,
      DEFAULT_CHARSET,
      OUT_DEFAULT_PRECIS,
      CLIP_DEFAULT_PRECIS,
      CLEARTYPE_QUALITY,
      DEFAULT_PITCH | FF_DONTCARE,
      "Segoe UI");

    TEXTMETRIC metric;
    SelectObject(font_data_.dc, font_data_.handle);
    GetTextMetricsA(font_data_.dc, &metric);
    font_.userdata = nk_handle_ptr(&font_data_);
    font_.height = static_cast<float>(metric.tmHeight);
    font_.width = get_text_width;

    // Set font.
    nk_style_set_font(&context, &font_);
    return {};
  }

  void end(ice::nuklear::color color) noexcept override
  {
    nk_end(&context);
    SetBkColor(memory, get_native_color(nk_rgb(color.r, color.g, color.b)));
    SelectObject(memory, GetStockObject(DC_PEN));
    SelectObject(memory, GetStockObject(DC_BRUSH));
    ExtTextOut(memory, 0, 0, ETO_OPAQUE, &rc, nullptr, 0, nullptr);
    const nk_command* command = nullptr;
    nk_foreach(command, &context)
    {
      switch (command->type) {
      case NK_COMMAND_NOP:
        break;
      case NK_COMMAND_SCISSOR:
        render(memory, *reinterpret_cast<const nk_command_scissor*>(command));
        break;
      case NK_COMMAND_LINE:
        render(memory, *reinterpret_cast<const nk_command_line*>(command));
        break;
      case NK_COMMAND_RECT:
        render(memory, *reinterpret_cast<const nk_command_rect*>(command));
        break;
      case NK_COMMAND_RECT_FILLED:
        render(memory, *reinterpret_cast<const nk_command_rect_filled*>(command));
        break;
      case NK_COMMAND_CIRCLE:
        render(memory, *reinterpret_cast<const nk_command_circle*>(command));
        break;
      case NK_COMMAND_CIRCLE_FILLED:
        render(memory, *reinterpret_cast<const nk_command_circle_filled*>(command));
        break;
      case NK_COMMAND_TRIANGLE:
        render(memory, *reinterpret_cast<const nk_command_triangle*>(command));
        break;
      case NK_COMMAND_TRIANGLE_FILLED:
        render(memory, *reinterpret_cast<const nk_command_triangle_filled*>(command));
        break;
      case NK_COMMAND_POLYGON:
        render(memory, *reinterpret_cast<const nk_command_polygon*>(command));
        break;
      case NK_COMMAND_POLYGON_FILLED:
        render(memory, *reinterpret_cast<const nk_command_polygon_filled*>(command));
        break;
      case NK_COMMAND_POLYLINE:
        render(memory, *reinterpret_cast<const nk_command_polyline*>(command));
        break;
      case NK_COMMAND_TEXT:
        render(memory, *reinterpret_cast<const nk_command_text*>(command));
        break;
      case NK_COMMAND_CURVE:
        render(memory, *reinterpret_cast<const nk_command_curve*>(command));
        break;
      case NK_COMMAND_IMAGE:
        render(memory, *reinterpret_cast<const nk_command_image*>(command));
        break;
      case NK_COMMAND_ARC:
      case NK_COMMAND_ARC_FILLED:
      case NK_COMMAND_RECT_MULTI_COLOR:
      default:
        break;
      }
    }
    BitBlt(window, 0, 0, rc.right, rc.bottom, memory, 0, 0, SRCCOPY);
    nk_clear(&context);
  }

private:
  struct font_data {
    HFONT handle{ nullptr };
    HDC dc{ nullptr };
  } font_data_;

  nk_user_font font_{};

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
    const auto size = MultiByteToWideChar(CP_UTF8, 0, c.string, c.length, nullptr, 0);
    const auto wstr = static_cast<WCHAR*>(_alloca(size * sizeof(wchar_t)));
    MultiByteToWideChar(CP_UTF8, 0, c.string, c.length, wstr, size);
    SetBkColor(dc, get_native_color(c.background));
    SetTextColor(dc, get_native_color(c.foreground));
    SelectObject(dc, static_cast<const font_data*>(c.font->userdata.ptr)->handle);
    ExtTextOutW(dc, c.x, c.y, ETO_OPAQUE, nullptr, wstr, static_cast<UINT>(size), nullptr);
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

  static float get_text_width(nk_handle handle, float height, const char* text, int length) noexcept
  {
    if (!handle.ptr || !text || length < 1) {
      return 0.0f;
    }
    SIZE size;
    if (GetTextExtentPoint32A(static_cast<font_data*>(handle.ptr)->dc, text, length, &size)) {
      return static_cast<float>(size.cx);
    }
    return -1.0f;
  }
};

}  // namespace ice::sys::windows::nuklear
