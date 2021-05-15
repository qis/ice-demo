#pragma once
#include "gdi.hpp"
#include <ice/format.hpp>
#include <ice/library.hpp>
#include <ice/os/nuklear.hpp>
#include <ice/ui/font.hpp>
#include <windows.h>
#include <objidl.h>
#include <shlwapi.h>
#include <algorithm>
#include <array>
#include <mutex>

#ifndef ICE_OS_WINDOWS_GDIPLUS
#  define ICE_OS_WINDOWS_GDIPLUS 1
#endif

#if ICE_OS_WINDOWS_GDIPLUS

namespace Gdiplus {

using std::min;
using std::max;

}  // namespace Gdiplus

#ifndef GDIPVER
#  define GDIPVER 0x0110
#endif

#include <gdiplus.h>

namespace ice::os::windows {

class gdiplus_library : public ice::library {
public:
  static inline std::mutex mutex;

  enum class errc : std::underlying_type_t<Gdiplus::Status>;

  static constexpr ice::error make_error(Gdiplus::Status status) noexcept
  {
    if (status) {
      if (status == Gdiplus::Status::Win32Error) {
        return ice::make_error<ice::system::errc>(GetLastError());
      }
      return static_cast<errc>(status);
    }
    return {};
  }

  static ice::error_info make_error_info(errc) noexcept
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
#if GDIPVER >= 0x0110
      case Gdiplus::Status::ProfileNotFound:
        return "profile not found";
#endif
      }
      return ice::error_info::format(code);
    };
    return { "gdiplus", text };
  }

  gdiplus_library() noexcept
    : ice::library("gdiplus")
  {}

  ~gdiplus_library() override
  {
    if (initialized_) {
      shutdown_(token_);
    }
  }

  ice::error create() noexcept
  {
    if (initialized_) {
      return {};
    }
    std::lock_guard lock{ mutex };
    if (initialized_) {
      return {};
    }
    if (auto e = open()) {
      ICE_TRACE_FORMAT("{}: {}", path(), e);
      return e;
    }
    if (!(startup_ = ice::library::get<decltype(startup_)>("GdiplusStartup"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GdiplusStartup: {}", path(), e);
      return e;
    }
    if (!(shutdown_ = ice::library::get<decltype(shutdown_)>("GdiplusShutdown"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(create_font = ice::library::get<decltype(create_font)>("GdipCreateFontFromDC"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(create_graphics = ice::library::get<decltype(create_graphics)>("GdipCreateFromHDC"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(create_pen = ice::library::get<decltype(create_pen)>("GdipCreatePen1"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(create_solid_fill = ice::library::get<decltype(create_solid_fill)>("GdipCreateSolidFill"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(delete_brush = ice::library::get<decltype(delete_brush)>("GdipDeleteBrush"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(delete_font = ice::library::get<decltype(delete_font)>("GdipDeleteFont"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(delete_graphics = ice::library::get<decltype(delete_graphics)>("GdipDeleteGraphics"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(delete_pen = ice::library::get<decltype(delete_pen)>("GdipDeletePen"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(delete_string_format = ice::library::get<decltype(delete_string_format)>("GdipDeleteStringFormat"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(draw_arc = ice::library::get<decltype(draw_arc)>("GdipDrawArcI"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(draw_bezier = ice::library::get<decltype(draw_bezier)>("GdipDrawBezierI"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(draw_ellipse = ice::library::get<decltype(draw_ellipse)>("GdipDrawEllipseI"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(draw_image_rect = ice::library::get<decltype(draw_image_rect)>("GdipDrawImageRectI"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(draw_line = ice::library::get<decltype(draw_line)>("GdipDrawLineI"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(draw_polygon = ice::library::get<decltype(draw_polygon)>("GdipDrawPolygonI"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(draw_rectangle = ice::library::get<decltype(draw_rectangle)>("GdipDrawRectangleI"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(draw_string = ice::library::get<decltype(draw_string)>("GdipDrawString"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(fill_ellipse = ice::library::get<decltype(fill_ellipse)>("GdipFillEllipseI"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(fill_pie = ice::library::get<decltype(fill_pie)>("GdipFillPieI"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(fill_polygon = ice::library::get<decltype(fill_polygon)>("GdipFillPolygonI"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(fill_rectangle = ice::library::get<decltype(fill_rectangle)>("GdipFillRectangleI"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(get_font_height = ice::library::get<decltype(get_font_height)>("GdipGetFontHeight"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(clear = ice::library::get<decltype(clear)>("GdipGraphicsClear"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(measure_string = ice::library::get<decltype(measure_string)>("GdipMeasureString"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(set_clip_rect = ice::library::get<decltype(set_clip_rect)>("GdipSetClipRectI"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(set_pen_color = ice::library::get<decltype(set_pen_color)>("GdipSetPenColor"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(set_pen_width = ice::library::get<decltype(set_pen_width)>("GdipSetPenWidth"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(set_smoothing_mode = ice::library::get<decltype(set_smoothing_mode)>("GdipSetSmoothingMode"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(set_solid_fill_color = ice::library::get<decltype(set_solid_fill_color)>("GdipSetSolidFillColor"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(set_string_format_flags = ice::library::get<decltype(set_string_format_flags)>("GdipSetStringFormatFlags"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    if (!(set_text_rendering_hint = ice::library::get<decltype(set_text_rendering_hint)>("GdipSetTextRenderingHint"))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    constexpr auto create_string_format_name = "GdipStringFormatGetGenericTypographic";
    if (!(create_string_format = ice::library::get<decltype(create_string_format)>(create_string_format_name))) {
      auto e = ice::make_error<ice::system::errc>(GetLastError());
      ICE_TRACE_FORMAT("{} GetProcAddress: {}", path(), e);
      return e;
    }
    Gdiplus::GdiplusStartupInput input{ nullptr, FALSE, TRUE };
    if (const auto code = startup_(&token_, &input, nullptr)) {
      auto e = make_error(code);
      ICE_TRACE_FORMAT("GdiplusStartup: {}", e);
      return e;
    }
    initialized_ = true;
    return {};
  }

  static std::shared_ptr<gdiplus_library> get_shared_object() noexcept
  {
    static std::weak_ptr<gdiplus_library> wp;
    auto sp = wp.lock();
    if (!sp) {
      std::lock_guard lock{ mutex };
      sp = wp.lock();
      if (!sp) {
        sp = std::make_shared<gdiplus_library>();
        wp = sp;
      }
    }
    return sp;
  }

  decltype(&Gdiplus::DllExports::GdipCreateFontFromDC) create_font{};                            // NOLINT
  decltype(&Gdiplus::DllExports::GdipCreateFromHDC) create_graphics{};                           // NOLINT
  decltype(&Gdiplus::DllExports::GdipCreatePen1) create_pen{};                                   // NOLINT
  decltype(&Gdiplus::DllExports::GdipCreateSolidFill) create_solid_fill{};                       // NOLINT
  decltype(&Gdiplus::DllExports::GdipDeleteBrush) delete_brush{};                                // NOLINT
  decltype(&Gdiplus::DllExports::GdipDeleteFont) delete_font{};                                  // NOLINT
  decltype(&Gdiplus::DllExports::GdipDeleteGraphics) delete_graphics{};                          // NOLINT
  decltype(&Gdiplus::DllExports::GdipDeletePen) delete_pen{};                                    // NOLINT
  decltype(&Gdiplus::DllExports::GdipDeleteStringFormat) delete_string_format{};                 // NOLINT
  decltype(&Gdiplus::DllExports::GdipDrawArcI) draw_arc{};                                       // NOLINT
  decltype(&Gdiplus::DllExports::GdipDrawBezierI) draw_bezier{};                                 // NOLINT
  decltype(&Gdiplus::DllExports::GdipDrawEllipseI) draw_ellipse{};                               // NOLINT
  decltype(&Gdiplus::DllExports::GdipDrawImageRectI) draw_image_rect{};                          // NOLINT
  decltype(&Gdiplus::DllExports::GdipDrawLineI) draw_line{};                                     // NOLINT
  decltype(&Gdiplus::DllExports::GdipDrawPolygonI) draw_polygon{};                               // NOLINT
  decltype(&Gdiplus::DllExports::GdipDrawRectangleI) draw_rectangle{};                           // NOLINT
  decltype(&Gdiplus::DllExports::GdipDrawString) draw_string{};                                  // NOLINT
  decltype(&Gdiplus::DllExports::GdipFillEllipseI) fill_ellipse{};                               // NOLINT
  decltype(&Gdiplus::DllExports::GdipFillPieI) fill_pie{};                                       // NOLINT
  decltype(&Gdiplus::DllExports::GdipFillPolygonI) fill_polygon{};                               // NOLINT
  decltype(&Gdiplus::DllExports::GdipFillRectangleI) fill_rectangle{};                           // NOLINT
  decltype(&Gdiplus::DllExports::GdipGetFontHeight) get_font_height{};                           // NOLINT
  decltype(&Gdiplus::DllExports::GdipGraphicsClear) clear{};                                     // NOLINT
  decltype(&Gdiplus::DllExports::GdipMeasureString) measure_string{};                            // NOLINT
  decltype(&Gdiplus::DllExports::GdipSetClipRectI) set_clip_rect{};                              // NOLINT
  decltype(&Gdiplus::DllExports::GdipSetPenColor) set_pen_color{};                               // NOLINT
  decltype(&Gdiplus::DllExports::GdipSetPenWidth) set_pen_width{};                               // NOLINT
  decltype(&Gdiplus::DllExports::GdipSetSmoothingMode) set_smoothing_mode{};                     // NOLINT
  decltype(&Gdiplus::DllExports::GdipSetSolidFillColor) set_solid_fill_color{};                  // NOLINT
  decltype(&Gdiplus::DllExports::GdipSetStringFormatFlags) set_string_format_flags{};            // NOLINT
  decltype(&Gdiplus::DllExports::GdipSetTextRenderingHint) set_text_rendering_hint{};            // NOLINT
  decltype(&Gdiplus::DllExports::GdipStringFormatGetGenericTypographic) create_string_format{};  // NOLINT

private:
  ULONG_PTR token_{};
  bool initialized_{ false };
  decltype(&Gdiplus::GdiplusStartup) startup_{};
  decltype(&Gdiplus::GdiplusShutdown) shutdown_{};
};

class gdiplus final : public ice::os::windows::context {
public:
  class font final : public ice::ui::font {
  public:
    font(gdiplus* gdiplus) noexcept
      : gdiplus_(gdiplus)
    {}

    ~font() override
    {
      ice::ui::font::set(nullptr);
      if (font_.userdata.ptr) {
        font_.userdata = nk_handle_ptr(nullptr);
      }
      if (library_) {
        if (graphics_) {
          library_->delete_graphics(graphics_);
        }
        if (handle_) {
          library_->delete_font(handle_);
        }
      }
    }

    ice::error create(HDC dc, const std::string& name, int size, int weight, ice::ui::font::flags flags) noexcept
    {
      ICE_ASSERT(!handle_);
      ICE_ASSERT(!graphics_);
      library_ = gdiplus_library::get_shared_object();
      if (auto e = library_->create()) {
        ICE_TRACE_FUNCTION;
        return e;
      }
      ice::os::windows::gdi::font font;
      if (auto e = font.create(dc, name, size, weight, flags)) {
        ICE_TRACE_FUNCTION;
        return e;
      }
      if (const auto code = library_->create_font(font.dc(), &handle_)) {
        auto e = gdiplus_library::make_error(code);
        ICE_TRACE_FORMAT("GdipCreateFontFromDC: {}", e);
        return e;
      }
      if (auto code = library_->create_graphics(dc, &graphics_)) {
        auto e = gdiplus_library::make_error(code);
        ICE_TRACE_FORMAT("GdipCreateFromHDC: {}", e);
        return e;
      }
      font_.userdata = nk_handle_ptr(this);
      if (const auto code = library_->get_font_height(handle_, graphics_, &font_.height)) {
        auto e = gdiplus_library::make_error(code);
        ICE_TRACE_FORMAT("GdipGetFontHeight: {}", e);
        return e;
      }
      font_.width = get_text_width;
      ice::ui::font::set(&font_);
      return {};
    }

    float width(std::string_view text, float height) const noexcept override
    {
      const auto wlength = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
      const auto wstring = static_cast<WCHAR*>(_alloca(wlength * sizeof(wchar_t)));
      MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), wstring, wlength);
      Gdiplus::RectF box{};
      const Gdiplus::RectF layout{ 0.0f, 0.0f, 65536.0f, 65536.0f };
      library_->measure_string(graphics_, wstring, wlength, handle_, &layout, gdiplus_->format_, &box, nullptr, nullptr);
      return box.Width;
    }

    void render(Gdiplus::GpGraphics* graphics, const nk_command_text& c) const noexcept
    {
      const auto wlength = MultiByteToWideChar(CP_UTF8, 0, c.string, c.length, nullptr, 0);
      const auto wstring = static_cast<WCHAR*>(_alloca(wlength * sizeof(wchar_t)));
      MultiByteToWideChar(CP_UTF8, 0, c.string, c.length, wstring, wlength);
      const Gdiplus::RectF layout = {
        static_cast<FLOAT>(c.x),
        static_cast<FLOAT>(c.y),
        static_cast<FLOAT>(c.w),
        static_cast<FLOAT>(c.h),
      };
      library_->set_solid_fill_color(gdiplus_->brush_, get_native_color(c.foreground));
      library_->draw_string(graphics, wstring, wlength, handle_, &layout, gdiplus_->format_, gdiplus_->brush_);
    }

  private:
    static float get_text_width(nk_handle handle, float height, const char* string, int length) noexcept
    {
      if (!handle.ptr || !string || length < 1) {
        return 0.0f;
      }
      return static_cast<const font*>(handle.ptr)->width({ string, static_cast<std::size_t>(length) }, height);
    }

    gdiplus* gdiplus_;
    nk_user_font font_{};
    Gdiplus::GpFont* handle_{};
    Gdiplus::GpGraphics* graphics_{};
    std::shared_ptr<gdiplus_library> library_;
  };

  gdiplus() noexcept
    : font_(this)
  {}

  ice::error create(HWND hwnd, std::string_view name, int size, int weight, ice::ui::font::flags flags) noexcept
  {
    library_ = gdiplus_library::get_shared_object();
    if (auto e = library_->create()) {
      ICE_TRACE_FUNCTION;
      return e;
    }

    if (auto code = library_->create_pen(0, 1.0f, Gdiplus::UnitPixel, &pen_)) {
      return gdiplus_library::make_error(code);
    }
    if (auto code = library_->create_solid_fill(0, &brush_)) {
      return gdiplus_library::make_error(code);
    }
    if (auto code = library_->create_string_format(&format_)) {
      return gdiplus_library::make_error(code);
    }

    INT string_format_flags = 0;
    string_format_flags |= Gdiplus::StringFormatFlagsNoFitBlackBox;
    string_format_flags |= Gdiplus::StringFormatFlagsMeasureTrailingSpaces;
    string_format_flags |= Gdiplus::StringFormatFlagsNoWrap;
    string_format_flags |= Gdiplus::StringFormatFlagsNoClip;
    if (auto code = library_->set_string_format_flags(format_, string_format_flags)) {
      return gdiplus_library::make_error(code);
    }
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
    auto font = std::make_shared<ice::os::windows::gdiplus::font>(this);
    if (auto e = font->create(window(), std::string{ name }, size, weight, flags)) {
      ICE_TRACE_FORMAT("Could not create GDI+ font: {}", e);
    }
    return font;
  }

  void render() noexcept override
  {
    Gdiplus::GpGraphics* graphics = nullptr;
    if (library_->create_graphics(memory(), &graphics)) {
      return;
    }
    library_->clear(graphics, Gdiplus::Color::MakeARGB(255, 30, 30, 30));
    library_->set_text_rendering_hint(graphics, Gdiplus::TextRenderingHintClearTypeGridFit);
    library_->set_smoothing_mode(graphics, Gdiplus::SmoothingModeHighQuality);
    const nk_command* cmd = nullptr;
    nk_context* ctx = get();
    nk_foreach(cmd, ctx)
    {
      switch (cmd->type) {
      case NK_COMMAND_NOP:
        break;
      case NK_COMMAND_SCISSOR:
        render(graphics, *reinterpret_cast<const nk_command_scissor*>(cmd));
        break;
      case NK_COMMAND_LINE:
        render(graphics, *reinterpret_cast<const nk_command_line*>(cmd));
        break;
      case NK_COMMAND_RECT:
        render(graphics, *reinterpret_cast<const nk_command_rect*>(cmd));
        break;
      case NK_COMMAND_RECT_FILLED:
        render(graphics, *reinterpret_cast<const nk_command_rect_filled*>(cmd));
        break;
      case NK_COMMAND_CIRCLE:
        render(graphics, *reinterpret_cast<const nk_command_circle*>(cmd));
        break;
      case NK_COMMAND_CIRCLE_FILLED:
        render(graphics, *reinterpret_cast<const nk_command_circle_filled*>(cmd));
        break;
      case NK_COMMAND_TRIANGLE:
        render(graphics, *reinterpret_cast<const nk_command_triangle*>(cmd));
        break;
      case NK_COMMAND_TRIANGLE_FILLED:
        render(graphics, *reinterpret_cast<const nk_command_triangle_filled*>(cmd));
        break;
      case NK_COMMAND_POLYGON:
        render(graphics, *reinterpret_cast<const nk_command_polygon*>(cmd));
        break;
      case NK_COMMAND_POLYGON_FILLED:
        render(graphics, *reinterpret_cast<const nk_command_polygon_filled*>(cmd));
        break;
      case NK_COMMAND_POLYLINE:
        render(graphics, *reinterpret_cast<const nk_command_polyline*>(cmd));
        break;
      case NK_COMMAND_TEXT:
        render(graphics, *reinterpret_cast<const nk_command_text*>(cmd));
        break;
      case NK_COMMAND_CURVE:
        render(graphics, *reinterpret_cast<const nk_command_curve*>(cmd));
        break;
      case NK_COMMAND_IMAGE:
        render(graphics, *reinterpret_cast<const nk_command_image*>(cmd));
        break;
      case NK_COMMAND_ARC:
      case NK_COMMAND_ARC_FILLED:
      case NK_COMMAND_RECT_MULTI_COLOR:
      default:
        break;
      }
    }
    const RECT* rc = rect();
    library_->delete_graphics(graphics);
    BitBlt(window(), 0, 0, rc->right, rc->bottom, memory(), 0, 0, SRCCOPY);
  }

private:
  void render(Gdiplus::GpGraphics* graphics, const nk_command_scissor& c) const noexcept
  {
    const auto w = static_cast<INT>(c.w);
    const auto h = static_cast<INT>(c.h);
    library_->set_clip_rect(graphics, c.x, c.y, w, h, Gdiplus::CombineModeReplace);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_line& c) noexcept
  {
    library_->set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_->set_pen_color(pen_, get_native_color(c.color));
    library_->draw_line(graphics, pen_, c.begin.x, c.begin.y, c.end.x, c.end.y);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_rect& c) noexcept
  {
    const auto w = static_cast<INT>(c.w);
    const auto h = static_cast<INT>(c.h);
    library_->set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_->set_pen_color(pen_, get_native_color(c.color));
    if (!c.rounding) {
      library_->draw_rectangle(graphics, pen_, c.x, c.y, w, h);
    } else {
      const auto r = static_cast<INT>(c.rounding);
      const auto d = 2 * r;
      library_->draw_arc(graphics, pen_, c.x, c.y, d, d, 180, 90);
      library_->draw_line(graphics, pen_, c.x + r, c.y, c.x + w - r, c.y);
      library_->draw_arc(graphics, pen_, c.x + w - d, c.y, d, d, 270, 90);
      library_->draw_line(graphics, pen_, c.x + w, c.y + r, c.x + w, c.y + h - r);
      library_->draw_arc(graphics, pen_, c.x + w - d, c.y + h - d, d, d, 0, 90);
      library_->draw_line(graphics, pen_, c.x, c.y + r, c.x, c.y + h - r);
      library_->draw_arc(graphics, pen_, c.x, c.y + h - d, d, d, 90, 90);
      library_->draw_line(graphics, pen_, c.x + r, c.y + h, c.x + w - r, c.y + h);
    }
  }
  void render(Gdiplus::GpGraphics* graphics, const nk_command_rect_filled& c) noexcept
  {
    const auto w = static_cast<INT>(c.w);
    const auto h = static_cast<INT>(c.h);
    library_->set_solid_fill_color(brush_, get_native_color(c.color));
    if (!c.rounding) {
      library_->fill_rectangle(graphics, brush_, c.x, c.y, w, h);
    } else {
      const auto r = static_cast<INT>(c.rounding);
      const auto d = 2 * r;
      library_->fill_rectangle(graphics, brush_, c.x + r, c.y, w - d, h);
      library_->fill_rectangle(graphics, brush_, c.x, c.y + r, r, h - d);
      library_->fill_rectangle(graphics, brush_, c.x + w - r, c.y + r, r, h - d);
      library_->fill_pie(graphics, brush_, c.x, c.y, d, d, 180, 90);
      library_->fill_pie(graphics, brush_, c.x + w - d, c.y, d, d, 270, 90);
      library_->fill_pie(graphics, brush_, c.x + w - d, c.y + h - d, d, d, 0, 90);
      library_->fill_pie(graphics, brush_, c.x, c.y + h - d, d, d, 90, 90);
    }
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_circle& c) noexcept
  {
    const auto w = static_cast<INT>(c.w);
    const auto h = static_cast<INT>(c.h);
    library_->set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_->set_pen_color(pen_, get_native_color(c.color));
    library_->draw_ellipse(graphics, pen_, c.x, c.y, w, h);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_circle_filled& c) noexcept
  {
    const auto w = static_cast<INT>(c.w);
    const auto h = static_cast<INT>(c.h);
    library_->set_solid_fill_color(brush_, get_native_color(c.color));
    library_->fill_ellipse(graphics, brush_, c.x, c.y, w, h);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_triangle& c) noexcept
  {
    const Gdiplus::Point points[] = {
      { c.a.x, c.a.y },
      { c.b.x, c.b.y },
      { c.c.x, c.c.y },
      { c.a.x, c.a.y },
    };
    library_->set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_->set_pen_color(pen_, get_native_color(c.color));
    library_->draw_polygon(graphics, pen_, points, 4);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_triangle_filled& c) noexcept
  {
    const Gdiplus::Point points[] = {
      { c.a.x, c.a.y },
      { c.b.x, c.b.y },
      { c.c.x, c.c.y },
    };
    library_->set_solid_fill_color(brush_, get_native_color(c.color));
    library_->fill_polygon(graphics, brush_, points, 3, Gdiplus::FillModeAlternate);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_polygon& c) noexcept
  {
    const auto& p = c.points;
    library_->set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_->set_pen_color(pen_, get_native_color(c.color));
    if (c.point_count) {
      for (unsigned short i = 1; i < c.point_count; i++) {
        library_->draw_line(graphics, pen_, p[i - 1].x, p[i - 1].y, p[i].x, p[i].y);
      }
      library_->draw_line(graphics, pen_, p[c.point_count - 1].x, p[c.point_count - 1].y, p[0].x, p[0].y);
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
    library_->set_solid_fill_color(brush_, get_native_color(c.color));
    library_->fill_polygon(graphics, brush_, points.data(), i, Gdiplus::FillModeAlternate);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_polyline& c) noexcept
  {
    const auto& p = c.points;
    library_->set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_->set_pen_color(pen_, get_native_color(c.color));
    if (c.point_count) {
      for (unsigned short i = 1; i < c.point_count; i++) {
        library_->draw_line(graphics, pen_, p[i - 1].x, p[i - 1].y, p[i].x, p[i].y);
      }
    }
  }

  static void render(Gdiplus::GpGraphics* graphics, const nk_command_text& c) noexcept
  {
    if (c.length < 1 || !c.font || !c.font->userdata.ptr) {
      return;
    }
    return static_cast<const font*>(c.font->userdata.ptr)->render(graphics, c);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_curve& c) noexcept
  {
    const Gdiplus::Point p[] = {
      { c.begin.x, c.begin.y },
      { c.ctrl[0].x, c.ctrl[0].y },
      { c.ctrl[1].x, c.ctrl[1].y },
      { c.end.x, c.end.y },
    };
    library_->set_pen_width(pen_, static_cast<Gdiplus::REAL>(c.line_thickness));
    library_->set_pen_color(pen_, get_native_color(c.color));
    library_->draw_bezier(graphics, pen_, p[0].X, p[0].Y, p[1].X, p[1].Y, p[2].X, p[2].Y, p[3].X, p[3].Y);
  }

  void render(Gdiplus::GpGraphics* graphics, const nk_command_image& c) const noexcept
  {
    const auto w = static_cast<INT>(c.w);
    const auto h = static_cast<INT>(c.h);
    const auto image = static_cast<Gdiplus::GpImage*>(c.img.handle.ptr);
    library_->draw_image_rect(graphics, image, c.x, c.y, w, h);
  }

  static constexpr Gdiplus::ARGB get_native_color(struct nk_color c) noexcept
  {
    return (c.a << 24) | (c.r << 16) | (c.g << 8) | c.b;
  }

  Gdiplus::GpPen* pen_{};
  Gdiplus::GpSolidFill* brush_{};
  Gdiplus::GpStringFormat* format_{};
  std::shared_ptr<gdiplus_library> library_;
  font font_;
};

}  // namespace ice::os::windows

#endif
