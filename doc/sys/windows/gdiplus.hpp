#pragma once
#include "config.hpp"
#include <ice/library.hpp>
#include <windows.h>
#include <objidl.h>
#include <shlwapi.h>
#include <algorithm>

#include <ice/format.hpp>

namespace Gdiplus {

using std::min;
using std::max;

}  // namespace Gdiplus

#include <gdiplus.h>

namespace ice::sys::windows::gdiplus {

enum class errc : std::underlying_type_t<Gdiplus::Status>;

constexpr ice::error make_error(Gdiplus::Status status) noexcept
{
  if (status == Gdiplus::Status::Win32Error) {
    return get_last_error();
  }
  return static_cast<errc>(status);
}

inline ice::error_info make_error_info(errc) noexcept
{
  constexpr auto text = [](int code) -> std::string {
    switch (static_cast<Gdiplus::Status>(code)) {
    case Gdiplus::Status::Ok:
      return "ok";
    case Gdiplus::Status::GenericError:
      return "generic error";
    case Gdiplus::Status::InvalidParameter:
      return "invalid parameter";
    case Gdiplus::Status::OutOfMemory:
      return "out of memory";
    case Gdiplus::Status::ObjectBusy:
      return "object busy";
    case Gdiplus::Status::InsufficientBuffer:
      return "insufficient buffer";
    case Gdiplus::Status::NotImplemented:
      return "not implemented";
    case Gdiplus::Status::Win32Error:
      return "windows error";
    case Gdiplus::Status::WrongState:
      return "wrong state";
    case Gdiplus::Status::Aborted:
      return "aborted";
    case Gdiplus::Status::FileNotFound:
      return "file not found";
    case Gdiplus::Status::ValueOverflow:
      return "value overflow";
    case Gdiplus::Status::AccessDenied:
      return "access denied";
    case Gdiplus::Status::UnknownImageFormat:
      return "unknown image format";
    case Gdiplus::Status::FontFamilyNotFound:
      return "font family not found";
    case Gdiplus::Status::FontStyleNotFound:
      return "font style not found";
    case Gdiplus::Status::NotTrueTypeFont:
      return "not truetype font";
    case Gdiplus::Status::UnsupportedGdiplusVersion:
      return "unsupported gdiplus version";
    case Gdiplus::Status::GdiplusNotInitialized:
      return "gdiplus not initialized";
    case Gdiplus::Status::PropertyNotFound:
      return "property not found";
    case Gdiplus::Status::PropertyNotSupported:
      return "property not supported";
#if (GDIPVER >= 0x0110)
    case Gdiplus::Status::ProfileNotFound:
      return "profile not found";
#endif  //(GDIPVER >= 0x0110)
    }
  };
  return { "gdiplus", text };
}

struct library : public ice::library {
  ULONG_PTR token{ 0 };

  library() noexcept : ice::library("gdiplus")
  {}

  ~library() override
  {
    if (shutdown && token) {
      shutdown(token);
    }
  }

  bool create() noexcept
  {
    if (auto e = open()) {
      ICE_TRACE_FORMAT("gdiplus.dll");
      return false;
    }
    if (!(startup = get<decltype(startup)>("GdiplusStartup"))) {
      ICE_TRACE_FORMAT("GdiplusStartup");
      return false;
    }
    if (!(shutdown = get<decltype(shutdown)>("GdiplusShutdown"))) {
      ICE_TRACE_FORMAT("GdiplusShutdown");
      return false;
    }
    if (!(create_font = get<decltype(create_font)>("GdipCreateFontFromDC"))) {
      ICE_TRACE_FORMAT("GdipCreateFontFromDC");
      return false;
    }
    if (!(create_graphics = get<decltype(create_graphics)>("GdipCreateFromHDC"))) {
      ICE_TRACE_FORMAT("GdipCreateFromHDC");
      return false;
    }
    if (!(create_pen = get<decltype(create_pen)>("GdipCreatePen1"))) {
      ICE_TRACE_FORMAT("GdipCreatePen1");
      return false;
    }
    if (!(create_solid_fill = get<decltype(create_solid_fill)>("GdipCreateSolidFill"))) {
      ICE_TRACE_FORMAT("GdipCreateSolidFill");
      return false;
    }
    if (!(delete_brush = get<decltype(delete_brush)>("GdipDeleteBrush"))) {
      ICE_TRACE_FORMAT("GdipDeleteBrush");
      return false;
    }
    if (!(delete_font = get<decltype(delete_font)>("GdipDeleteFont"))) {
      ICE_TRACE_FORMAT("GdipDeleteFont");
      return false;
    }
    if (!(delete_graphics = get<decltype(delete_graphics)>("GdipDeleteGraphics"))) {
      ICE_TRACE_FORMAT("GdipDeleteGraphics");
      return false;
    }
    if (!(delete_pen = get<decltype(delete_pen)>("GdipDeletePen"))) {
      ICE_TRACE_FORMAT("GdipDeletePen");
      return false;
    }
    if (!(delete_string_format = get<decltype(delete_string_format)>("GdipDeleteStringFormat"))) {
      ICE_TRACE_FORMAT("GdipDeleteStringFormat");
      return false;
    }
    if (!(draw_arc = get<decltype(draw_arc)>("GdipDrawArcI"))) {
      ICE_TRACE_FORMAT("GdipDrawArcI");
      return false;
    }
    if (!(draw_bezier = get<decltype(draw_bezier)>("GdipDrawBezierI"))) {
      ICE_TRACE_FORMAT("GdipDrawBezierI");
      return false;
    }
    if (!(draw_ellipse = get<decltype(draw_ellipse)>("GdipDrawEllipseI"))) {
      ICE_TRACE_FORMAT("GdipDrawEllipseI");
      return false;
    }
    if (!(draw_image_rect = get<decltype(draw_image_rect)>("GdipDrawImageRectI"))) {
      ICE_TRACE_FORMAT("GdipDrawImageRectI");
      return false;
    }
    if (!(draw_line = get<decltype(draw_line)>("GdipDrawLineI"))) {
      ICE_TRACE_FORMAT("GdipDrawLineI");
      return false;
    }
    if (!(draw_polygon = get<decltype(draw_polygon)>("GdipDrawPolygonI"))) {
      ICE_TRACE_FORMAT("GdipDrawPolygonI");
      return false;
    }
    if (!(draw_rectangle = get<decltype(draw_rectangle)>("GdipDrawRectangleI"))) {
      ICE_TRACE_FORMAT("GdipDrawRectangleI");
      return false;
    }
    if (!(draw_string = get<decltype(draw_string)>("GdipDrawString"))) {
      ICE_TRACE_FORMAT("GdipDrawString");
      return false;
    }
    if (!(fill_ellipse = get<decltype(fill_ellipse)>("GdipFillEllipseI"))) {
      ICE_TRACE_FORMAT("GdipFillEllipseI");
      return false;
    }
    if (!(fill_pie = get<decltype(fill_pie)>("GdipFillPieI"))) {
      ICE_TRACE_FORMAT("GdipFillPieI");
      return false;
    }
    if (!(fill_polygon = get<decltype(fill_polygon)>("GdipFillPolygonI"))) {
      ICE_TRACE_FORMAT("GdipFillPolygonI");
      return false;
    }
    if (!(fill_rectangle = get<decltype(fill_rectangle)>("GdipFillRectangleI"))) {
      ICE_TRACE_FORMAT("GdipFillRectangleI");
      return false;
    }
    if (!(get_font_height = get<decltype(get_font_height)>("GdipGetFontHeight"))) {
      ICE_TRACE_FORMAT("GdipGetFontHeight");
      return false;
    }
    if (!(clear = get<decltype(clear)>("GdipGraphicsClear"))) {
      ICE_TRACE_FORMAT("GdipGraphicsClear");
      return false;
    }
    if (!(measure_string = get<decltype(measure_string)>("GdipMeasureString"))) {
      ICE_TRACE_FORMAT("GdipMeasureString");
      return false;
    }
    if (!(set_clip_rect = get<decltype(set_clip_rect)>("GdipSetClipRectI"))) {
      ICE_TRACE_FORMAT("GdipSetClipRectI");
      return false;
    }
    if (!(set_pen_color = get<decltype(set_pen_color)>("GdipSetPenColor"))) {
      ICE_TRACE_FORMAT("GdipSetPenColor");
      return false;
    }
    if (!(set_pen_width = get<decltype(set_pen_width)>("GdipSetPenWidth"))) {
      ICE_TRACE_FORMAT("GdipSetPenWidth");
      return false;
    }
    if (!(set_smoothing_mode = get<decltype(set_smoothing_mode)>("GdipSetSmoothingMode"))) {
      ICE_TRACE_FORMAT("GdipSetSmoothingMode");
      return false;
    }
    if (!(set_solid_fill_color = get<decltype(set_solid_fill_color)>("GdipSetSolidFillColor"))) {
      ICE_TRACE_FORMAT("GdipSetSolidFillColor");
      return false;
    }
    if (!(set_string_format_flags = get<decltype(set_string_format_flags)>("GdipSetStringFormatFlags"))) {
      ICE_TRACE_FORMAT("GdipSetStringFormatFlags");
      return false;
    }
    if (!(set_text_rendering_hint = get<decltype(set_text_rendering_hint)>("GdipSetTextRenderingHint"))) {
      ICE_TRACE_FORMAT("GdipSetTextRenderingHint");
      return false;
    }
    if (!(string_format_get_generic_typographic = get<decltype(string_format_get_generic_typographic)>("GdipStringFormatGetGenericTypographic"))) {
      ICE_TRACE_FORMAT("GdipStringFormatGetGenericTypographic");
      return false;
    }
    Gdiplus::GdiplusStartupInput input{ nullptr, FALSE, TRUE };
    return startup(&token, &input, nullptr) == Gdiplus::Status::Ok;
  }

  // clang-format off
  decltype(&Gdiplus::GdiplusStartup)                                     startup_{};
  decltype(&Gdiplus::GdiplusShutdown)                                    shutdown_{};
  decltype(&Gdiplus::DllExports::GdipCreateFontFromDC)                   create_font_{};
  decltype(&Gdiplus::DllExports::GdipCreateFromHDC)                      create_graphics_{};
  decltype(&Gdiplus::DllExports::GdipCreatePen1)                         create_pen_{};
  decltype(&Gdiplus::DllExports::GdipCreateSolidFill)                    create_solid_fill_{};
  decltype(&Gdiplus::DllExports::GdipDeleteBrush)                        delete_brush_{};
  decltype(&Gdiplus::DllExports::GdipDeleteFont)                         delete_font_{};
  decltype(&Gdiplus::DllExports::GdipDeleteGraphics)                     delete_graphics_{};
  decltype(&Gdiplus::DllExports::GdipDeletePen)                          delete_pen_{};
  decltype(&Gdiplus::DllExports::GdipDeleteStringFormat)                 delete_string_format_{};
  decltype(&Gdiplus::DllExports::GdipDrawArcI)                           draw_arc_{};
  decltype(&Gdiplus::DllExports::GdipDrawBezierI)                        draw_bezier_{};
  decltype(&Gdiplus::DllExports::GdipDrawEllipseI)                       draw_ellipse_{};
  decltype(&Gdiplus::DllExports::GdipDrawImageRectI)                     draw_image_rect_{};
  decltype(&Gdiplus::DllExports::GdipDrawLineI)                          draw_line_{};
  decltype(&Gdiplus::DllExports::GdipDrawPolygonI)                       draw_polygon_{};
  decltype(&Gdiplus::DllExports::GdipDrawRectangleI)                     draw_rectangle_{};
  decltype(&Gdiplus::DllExports::GdipDrawString)                         draw_string_{};
  decltype(&Gdiplus::DllExports::GdipFillEllipseI)                       fill_ellipse_{};
  decltype(&Gdiplus::DllExports::GdipFillPieI)                           fill_pie_{};
  decltype(&Gdiplus::DllExports::GdipFillPolygonI)                       fill_polygon_{};
  decltype(&Gdiplus::DllExports::GdipFillRectangleI)                     fill_rectangle_{};
  decltype(&Gdiplus::DllExports::GdipGetFontHeight)                      get_font_height_{};
  decltype(&Gdiplus::DllExports::GdipGraphicsClear)                      clear_{};
  decltype(&Gdiplus::DllExports::GdipMeasureString)                      measure_string_{};
  decltype(&Gdiplus::DllExports::GdipSetClipRectI)                       set_clip_rect_{};
  decltype(&Gdiplus::DllExports::GdipSetPenColor)                        set_pen_color_{};
  decltype(&Gdiplus::DllExports::GdipSetPenWidth)                        set_pen_width_{};
  decltype(&Gdiplus::DllExports::GdipSetSmoothingMode)                   set_smoothing_mode_{};
  decltype(&Gdiplus::DllExports::GdipSetSolidFillColor)                  set_solid_fill_color_{};
  decltype(&Gdiplus::DllExports::GdipSetStringFormatFlags)               set_string_format_flags_{};
  decltype(&Gdiplus::DllExports::GdipSetTextRenderingHint)               set_text_rendering_hint_{};
  decltype(&Gdiplus::DllExports::GdipStringFormatGetGenericTypographic)  string_format_get_generic_typographic_{};
  // clang-format on
};

}  // namespace ice::sys::windows::gdiplus
