#include "context.hpp"
#include <ice/os/nuklear.hpp>

namespace ice::ui {

void context::layout_row_dynamic(float height, int cols) noexcept
{
  nk_layout_row_dynamic(context_, height, cols);
}

void context::layout_row_static(float height, int item_width, int cols) noexcept
{
  nk_layout_row_static(context_, height, item_width, cols);
}

bool context::button_label(const char* text) noexcept
{
  return nk_button_label(context_, text);
}

bool context::option_label(const char* text, bool active) noexcept
{
  return nk_option_label(context_, text, active);
}

void context::property_int(const char* text, int min, int* val, int max, int step, float inc_per_pixel) noexcept
{
  return nk_property_int(context_, text, min, val, max, step, inc_per_pixel);
}

nk_context* context::get() const noexcept
{
  return context_;
}

void context::set(nk_context* context) noexcept
{
  context_ = context;
}

}  // namespace ice::ui
