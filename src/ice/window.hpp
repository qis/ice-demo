#pragma once
#include <ice/ui/font.hpp>
#include <memory>

namespace ice {

// ================================================================================================
// window interfaces
// ================================================================================================

namespace os {
class window;
}  // namespace os

namespace ui {
class context;
}  // namespace ui

namespace vk {
class context;
}  // namespace vk

// ================================================================================================
// window
// ================================================================================================

class ICE_API window {
public:
  window() noexcept = default;
  window(window&& other) noexcept;
  window(const window& other) = delete;
  window& operator=(window&& other) noexcept;
  window& operator=(const window& other) = delete;
  virtual ~window();

  ice::error create() noexcept;
  ice::error destroy() noexcept;

  enum class mode {
    windowed = 1 << 0,
    fullscreen = 1 << 1,
  };

  ice::error set(ice::window::mode mode) noexcept;

  enum class style {
    none = 0,
    border = 1 << 0,
    titlebar = 1 << 1,
    minimize = 1 << 2,
    maximize = 1 << 3,
    close = 1 << 4,
  };

  ice::error set(ice::window::style style) noexcept;

  ice::error text(std::string_view text) noexcept;
  ice::error icon(std::string_view icon) noexcept;

  ice::error start(std::string_view name = {}, int size = 9, int weight = 400,
    ice::ui::font::flags flags = ice::ui::font::flags::normal) noexcept;

  ice::error show() noexcept;
  ice::error hide() noexcept;

  ice::error minimize() noexcept;
  ice::error maximize() noexcept;
  ice::error restore() noexcept;

  ice::error run() noexcept;

  virtual void on_create() noexcept;

  virtual void on_render(ice::ui::context& context) noexcept;
  virtual void on_render(ice::vk::context& context) noexcept;

  virtual void on_destroy() noexcept;
  virtual void on_close() noexcept;

private:
  std::shared_ptr<ice::os::window> window_;
};

constexpr ice::window::mode operator|(ice::window::mode lhs, ice::window::mode rhs) noexcept
{
  return static_cast<ice::window::mode>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

constexpr ice::window::style operator|(ice::window::style lhs, ice::window::style rhs) noexcept
{
  return static_cast<ice::window::style>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

}  // namespace ice
