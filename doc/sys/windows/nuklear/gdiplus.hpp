#pragma once
#include "renderer.hpp"
#include <ice/sys/windows/gdiplus.hpp>

namespace ice::sys::windows::nuklear {

class gdiplus final : public renderer {
public:
  gdiplus(ice::sys::windows::gdiplus::library& library) noexcept
    : library_(library)
  {}

  ~gdiplus() override
  {
    if (font_.userdata.ptr) {
      font_.userdata = nk_handle_ptr(nullptr);
    }
    if (font_data_.handle) {
      library_.delete_font(font_data_.handle);
      font_data_.handle = nullptr;
    }
    if (format_) {
      library_.delete_string_format(format_);
      format_ = nullptr;
    }
    if (brush_) {
      library_.delete_brush(brush_);
      brush_ = nullptr;
    }
    if (pen_) {
      library_.delete_pen(pen_);
      pen_ = nullptr;
    }
  }

  ice::error create(HWND hwnd) noexcept
  {
    renderer::create(hwnd);

    // Create GDI+ objects.
    if (auto code = library_.create_pen(0, 1.0f, Gdiplus::UnitPixel, &pen_)) {
      return ice::sys::windows::gdiplus::make_error(code);
    }
    if (auto code = library_.create_solid_fill(0, &brush_)) {
      return ice::sys::windows::gdiplus::make_error(code);
    }
    if (auto code = library_.string_format_get_generic_typographic(&format_)) {
      return ice::sys::windows::gdiplus::make_error(code);
    }

    auto flags = 0;
    flags |= Gdiplus::StringFormatFlagsNoFitBlackBox;
    flags |= Gdiplus::StringFormatFlagsMeasureTrailingSpaces;
    flags |= Gdiplus::StringFormatFlagsNoWrap;
    flags |= Gdiplus::StringFormatFlagsNoClip;
    if (auto code = library_.set_string_format_flags(format_, flags)) {
      return ice::sys::windows::gdiplus::make_error(code);
    }

    // Create font.
    font_data_.self = this;
    const auto dc = CreateCompatibleDC(nullptr);
    const auto handle = CreateFontA(-MulDiv(9, GetDeviceCaps(window, LOGPIXELSY), 72),
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
    SelectObject(dc, handle);
    if (const auto code = library_.create_font(dc, &font_data_.handle)) {
      DeleteObject(handle);
      DeleteDC(dc);
      return ice::sys::windows::gdiplus::make_error(code);
    }
    DeleteObject(handle);
    DeleteDC(dc);

    font_.userdata = nk_handle_ptr(&font_data_);

    Gdiplus::GpGraphics* graphics = nullptr;
    if (auto code = library_.create_graphics(memory, &graphics)) {
      return ice::sys::windows::gdiplus::make_error(code);
    }
    if (auto code = library_.get_font_height(font_data_.handle, graphics, &font_.height)) {
      library_.delete_graphics(graphics);
      return ice::sys::windows::gdiplus::make_error(code);
    }
    library_.delete_graphics(graphics);
    font_.width = get_text_width;

    // Set font.
    nk_style_set_font(&context, &font_);
    return {};
  }

  void end(ice::nuklear::color color) noexcept override
  {
    nk_end(&context);
    Gdiplus::GpGraphics* graphics = nullptr;
    if (library_.create_graphics(memory, &graphics)) {
      return;
    }
    library_.clear(graphics, Gdiplus::Color::MakeARGB(255, color.r, color.g, color.b));
    library_.set_text_rendering_hint(graphics, Gdiplus::TextRenderingHintClearTypeGridFit);
    library_.set_smoothing_mode(graphics, Gdiplus::SmoothingModeHighQuality);
    const nk_command* command = nullptr;
    nk_foreach(command, &context)
    {
      switch (command->type) {
      case NK_COMMAND_NOP:
        break;
      case NK_COMMAND_SCISSOR:
        render(graphics, *reinterpret_cast<const nk_command_scissor*>(command));
        break;
      case NK_COMMAND_LINE:
        render(graphics, *reinterpret_cast<const nk_command_line*>(command));
        break;
      case NK_COMMAND_RECT:
        render(graphics, *reinterpret_cast<const nk_command_rect*>(command));
        break;
      case NK_COMMAND_RECT_FILLED:
        render(graphics, *reinterpret_cast<const nk_command_rect_filled*>(command));
        break;
      case NK_COMMAND_CIRCLE:
        render(graphics, *reinterpret_cast<const nk_command_circle*>(command));
        break;
      case NK_COMMAND_CIRCLE_FILLED:
        render(graphics, *reinterpret_cast<const nk_command_circle_filled*>(command));
        break;
      case NK_COMMAND_TRIANGLE:
        render(graphics, *reinterpret_cast<const nk_command_triangle*>(command));
        break;
      case NK_COMMAND_TRIANGLE_FILLED:
        render(graphics, *reinterpret_cast<const nk_command_triangle_filled*>(command));
        break;
      case NK_COMMAND_POLYGON:
        render(graphics, *reinterpret_cast<const nk_command_polygon*>(command));
        break;
      case NK_COMMAND_POLYGON_FILLED:
        render(graphics, *reinterpret_cast<const nk_command_polygon_filled*>(command));
        break;
      case NK_COMMAND_POLYLINE:
        render(graphics, *reinterpret_cast<const nk_command_polyline*>(command));
        break;
      case NK_COMMAND_TEXT:
        render(graphics, *reinterpret_cast<const nk_command_text*>(command));
        break;
      case NK_COMMAND_CURVE:
        render(graphics, *reinterpret_cast<const nk_command_curve*>(command));
        break;
      case NK_COMMAND_IMAGE:
        render(graphics, *reinterpret_cast<const nk_command_image*>(command));
        break;
      case NK_COMMAND_ARC:
      case NK_COMMAND_ARC_FILLED:
      case NK_COMMAND_RECT_MULTI_COLOR:
      default:
        break;
      }
    }
    library_.delete_graphics(graphics);
    BitBlt(window, 0, 0, rc.right, rc.bottom, memory, 0, 0, SRCCOPY);
    nk_clear(&context);
  }

private:
  ice::sys::windows::gdiplus::library& library_;

  Gdiplus::GpPen* pen_{ nullptr };
  Gdiplus::GpSolidFill* brush_{ nullptr };
  Gdiplus::GpStringFormat* format_{ nullptr };

  struct font_data {
    Gdiplus::GpFont* handle{ nullptr };
    gdiplus* self{ nullptr };
  } font_data_;

  nk_user_font font_{};

  void render(Gdiplus::GpGraphics* graphics, const nk_command_scissor& c) const noexcept
  {
    const auto w = static_cast<INT>(c.w);
    const auto h = static_cast<INT>(c.h);
    library_.set_clip_rect(graphics, c.x, c.y, w, h, Gdiplus::CombineModeReplace);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_line& c) noexcept
  {
    library_.set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_.set_pen_color(pen_, get_native_color(c.color));
    library_.draw_line(graphics, pen_, c.begin.x, c.begin.y, c.end.x, c.end.y);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_rect& c) noexcept
  {
    const auto w = static_cast<INT>(c.w);
    const auto h = static_cast<INT>(c.h);
    library_.set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_.set_pen_color(pen_, get_native_color(c.color));
    if (!c.rounding) {
      library_.draw_rectangle(graphics, pen_, c.x, c.y, w, h);
    } else {
      const auto r = static_cast<INT>(c.rounding);
      const auto d = 2 * r;
      library_.draw_arc(graphics, pen_, c.x, c.y, d, d, 180, 90);
      library_.draw_line(graphics, pen_, c.x + r, c.y, c.x + w - r, c.y);
      library_.draw_arc(graphics, pen_, c.x + w - d, c.y, d, d, 270, 90);
      library_.draw_line(graphics, pen_, c.x + w, c.y + r, c.x + w, c.y + h - r);
      library_.draw_arc(graphics, pen_, c.x + w - d, c.y + h - d, d, d, 0, 90);
      library_.draw_line(graphics, pen_, c.x, c.y + r, c.x, c.y + h - r);
      library_.draw_arc(graphics, pen_, c.x, c.y + h - d, d, d, 90, 90);
      library_.draw_line(graphics, pen_, c.x + r, c.y + h, c.x + w - r, c.y + h);
    }
  }
  void render(Gdiplus::GpGraphics* graphics, const nk_command_rect_filled& c) noexcept
  {
    const auto w = static_cast<INT>(c.w);
    const auto h = static_cast<INT>(c.h);
    library_.set_solid_fill_color(brush_, get_native_color(c.color));
    if (!c.rounding) {
      library_.fill_rectangle(graphics, brush_, c.x, c.y, w, h);
    } else {
      const auto r = static_cast<INT>(c.rounding);
      const auto d = 2 * r;
      library_.fill_rectangle(graphics, brush_, c.x + r, c.y, w - d, h);
      library_.fill_rectangle(graphics, brush_, c.x, c.y + r, r, h - d);
      library_.fill_rectangle(graphics, brush_, c.x + w - r, c.y + r, r, h - d);
      library_.fill_pie(graphics, brush_, c.x, c.y, d, d, 180, 90);
      library_.fill_pie(graphics, brush_, c.x + w - d, c.y, d, d, 270, 90);
      library_.fill_pie(graphics, brush_, c.x + w - d, c.y + h - d, d, d, 0, 90);
      library_.fill_pie(graphics, brush_, c.x, c.y + h - d, d, d, 90, 90);
    }
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_circle& c) noexcept
  {
    const auto w = static_cast<INT>(c.w);
    const auto h = static_cast<INT>(c.h);
    library_.set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_.set_pen_color(pen_, get_native_color(c.color));
    library_.draw_ellipse(graphics, pen_, c.x, c.y, w, h);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_circle_filled& c) noexcept
  {
    const auto w = static_cast<INT>(c.w);
    const auto h = static_cast<INT>(c.h);
    library_.set_solid_fill_color(brush_, get_native_color(c.color));
    library_.fill_ellipse(graphics, brush_, c.x, c.y, w, h);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_triangle& c) noexcept
  {
    const Gdiplus::Point points[] = {
      { c.a.x, c.a.y },
      { c.b.x, c.b.y },
      { c.c.x, c.c.y },
      { c.a.x, c.a.y },
    };
    library_.set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_.set_pen_color(pen_, get_native_color(c.color));
    library_.draw_polygon(graphics, pen_, points, 4);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_triangle_filled& c) noexcept
  {
    const Gdiplus::Point points[] = {
      { c.a.x, c.a.y },
      { c.b.x, c.b.y },
      { c.c.x, c.c.y },
    };
    library_.set_solid_fill_color(brush_, get_native_color(c.color));
    library_.fill_polygon(graphics, brush_, points, 3, Gdiplus::FillModeAlternate);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_polygon& c) noexcept
  {
    const auto& p = c.points;
    library_.set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_.set_pen_color(pen_, get_native_color(c.color));
    if (c.point_count) {
      for (unsigned short i = 1; i < c.point_count; i++) {
        library_.draw_line(graphics, pen_, p[i - 1].x, p[i - 1].y, p[i].x, p[i].y);
      }
      library_.draw_line(graphics, pen_, p[c.point_count - 1].x, p[c.point_count - 1].y, p[0].x, p[0].y);
    }
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_polygon_filled& c) noexcept
  {
    unsigned short i = 0;
    std::array<Gdiplus::Point, 64> points;
    for (i = 0; i < c.point_count && i < points.size(); i++) {
      points[i].X = c.points[i].x;
      points[i].Y = c.points[i].y;
    }
    library_.set_solid_fill_color(brush_, get_native_color(c.color));
    library_.fill_polygon(graphics, brush_, points.data(), i, Gdiplus::FillModeAlternate);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_polyline& c) noexcept
  {
    const auto& p = c.points;
    library_.set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_.set_pen_color(pen_, get_native_color(c.color));
    if (c.point_count) {
      for (unsigned short i = 1; i < c.point_count; i++) {
        library_.draw_line(graphics, pen_, p[i - 1].x, p[i - 1].y, p[i].x, p[i].y);
      }
    }
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_text& c) noexcept
  {
    if (c.length < 1 || !c.font || !c.font->userdata.ptr) {
      return;
    }
    const Gdiplus::RectF layout = {
      static_cast<FLOAT>(c.x),
      static_cast<FLOAT>(c.y),
      static_cast<FLOAT>(c.w),
      static_cast<FLOAT>(c.h),
    };
    const auto data = static_cast<const font_data*>(c.font->userdata.ptr);
    const auto size = MultiByteToWideChar(CP_UTF8, 0, c.string, c.length, nullptr, 0);
    const auto wstr = static_cast<WCHAR*>(_alloca(size * sizeof(wchar_t)));
    MultiByteToWideChar(CP_UTF8, 0, c.string, c.length, wstr, size);
    library_.set_solid_fill_color(brush_, get_native_color(c.foreground));
    library_.draw_string(graphics, wstr, size, data->handle, &layout, format_, brush_);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_curve& c) noexcept
  {
    const Gdiplus::Point p[] = {
      { c.begin.x, c.begin.y },
      { c.ctrl[0].x, c.ctrl[0].y },
      { c.ctrl[1].x, c.ctrl[1].y },
      { c.end.x, c.end.y },
    };
    library_.set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_.set_pen_color(pen_, get_native_color(c.color));
    library_.draw_bezier(graphics, pen_, p[0].X, p[0].Y, p[1].X, p[1].Y, p[2].X, p[2].Y, p[3].X, p[3].Y);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_image& c) const noexcept
  {
    const auto w = static_cast<INT>(c.w);
    const auto h = static_cast<INT>(c.h);
    const auto image = static_cast<Gdiplus::GpImage*>(c.img.handle.ptr);
    library_.draw_image_rect(graphics, image, c.x, c.y, w, h);
  }

  static constexpr Gdiplus::ARGB get_native_color(struct nk_color c) noexcept
  {
    return (c.a << 24) | (c.r << 16) | (c.g << 8) | c.b;
  }

  static float get_text_width(nk_handle handle, float height, const char* string, int length) noexcept
  {
    if (!handle.ptr || !string || length < 1) {
      return 0.0f;
    }
    const auto data = static_cast<const font_data*>(handle.ptr);
    const auto size = MultiByteToWideChar(CP_UTF8, 0, string, length, nullptr, 0);
    const auto wstr = static_cast<WCHAR*>(_alloca(size * sizeof(wchar_t)));
    MultiByteToWideChar(CP_UTF8, 0, string, length, wstr, size);
    const auto memory = data->self->memory;
    const auto format = data->self->format_;
    const Gdiplus::RectF layout{ 0.0f, 0.0f, 65536.0f, 65536.0f };
    Gdiplus::RectF box{};
    Gdiplus::GpGraphics* graphics = nullptr;
    const auto& library = data->self->library_;
    if (library.create_graphics(memory, &graphics)) {
      return 0.0f;
    }
    library.measure_string(graphics, wstr, size, data->handle, &layout, format, &box, nullptr, nullptr);
    library.delete_graphics(graphics);
    return box.Width;
  }
};

}  // namespace ice::sys::windows::nuklear
