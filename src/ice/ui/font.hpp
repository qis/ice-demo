#pragma once
#include <ice/error.hpp>

extern "C" struct nk_user_font;

namespace ice::ui {

class ICE_API font {
public:
  enum class flags {
    normal = 0,
    italic = (1 << 0),
    underline = (1 << 1),
    strikeout = (1 << 2),
  };

  virtual ~font() = default;

  virtual float width(std::string_view text, float height) const noexcept = 0;

  float height() const noexcept;

  nk_user_font* get() const noexcept;

protected:
  void set(nk_user_font* font) noexcept;

private:
  nk_user_font* font_{ nullptr };
};

constexpr ice::ui::font::flags operator|(ice::ui::font::flags lhs, ice::ui::font::flags rhs) noexcept
{
  return static_cast<ice::ui::font::flags>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

constexpr bool operator&(ice::ui::font::flags lhs, ice::ui::font::flags rhs) noexcept
{
  return static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs);
}

}  // namespace ice::ui
