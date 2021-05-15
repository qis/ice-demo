#include "font.hpp"
#include <ice/os/nuklear.hpp>

namespace ice::ui {

float font::height() const noexcept
{
  return font_ ? font_->height : -1.0f;
}

nk_user_font* font::get() const noexcept
{
  return font_;
}

void font::set(nk_user_font* font) noexcept
{
  font_ = font;
}


}  // namespace ice::ui
