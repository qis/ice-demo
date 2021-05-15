#pragma once
#include <ice/ui/font.hpp>
#include <memory>

extern "C" struct nk_context;

namespace ice::ui {

class ICE_API context {
public:
  context() noexcept = default;
  virtual ~context() = default;

  virtual float cx() const noexcept = 0;
  virtual float cy() const noexcept = 0;

  virtual std::shared_ptr<ice::ui::font> create_font(std::string_view name, int size, int weight = 400,
    ice::ui::font::flags flags = ice::ui::font::flags::normal) noexcept = 0;

  void layout_row_dynamic(float height, int cols) noexcept;
  void layout_row_static(float height, int item_width, int cols) noexcept;
  bool button_label(const char* text) noexcept;
  bool option_label(const char* text, bool active) noexcept;
  void property_int(const char* text, int min, int* val, int max, int step, float inc_per_pixel) noexcept;

  nk_context* get() const noexcept;

protected:
  void set(nk_context* context) noexcept;

private:
  nk_context* context_{ nullptr };
};

}  // namespace ice::ui
