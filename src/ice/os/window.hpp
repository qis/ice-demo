#pragma once
#include <ice/window.hpp>

namespace ice::os {

class window {
public:
  virtual ~window() = default;
  virtual void move(ice::window* window = nullptr) noexcept = 0;
  virtual ice::error create() noexcept = 0;
  virtual ice::error destroy() noexcept = 0;
  virtual ice::error set(ice::window::mode mode) noexcept = 0;
  virtual ice::error set(ice::window::style style) noexcept = 0;
  virtual ice::error text(std::string_view text) noexcept = 0;
  virtual ice::error icon(std::string_view icon) noexcept = 0;
  virtual ice::error start(std::string_view name, int size, int weight, ice::ui::font::flags flags) noexcept = 0;
  virtual ice::error show() noexcept = 0;
  virtual ice::error hide() noexcept = 0;
  virtual ice::error minimize() noexcept = 0;
  virtual ice::error maximize() noexcept = 0;
  virtual ice::error restore() noexcept = 0;
  virtual ice::error run() noexcept = 0;
};

}  // namespace ice::os
