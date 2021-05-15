#include "window.hpp"

#ifdef _WIN32
#  include <ice/os/windows/window.hpp>
#else
#  include <ice/os/xcb/window.hpp>
#endif

namespace ice {

window::window(window&& other) noexcept
{
  window_ = other.window_;
  if (window_) {
    window_->move(this);
    other.window_.reset();
  }
}

window& window::operator=(window&& other) noexcept
{
  if (window_) {
    window_->move();
  }
  window_ = other.window_;
  if (window_) {
    window_->move(this);
    other.window_.reset();
  }
  return *this;
}

window::~window()
{
  if (window_) {
    window_->move();
  }
}

ice::error window::create() noexcept
{
  if (window_) {
    return ice::errc::not_available;
  }
#ifdef _WIN32
  window_ = std::make_shared<ice::os::windows::window>();
#else
  window_ = std::make_shared<ice::os::xcb::window>();
#endif
  window_->move(this);
  return window_->create();
}

ice::error window::destroy() noexcept
{
  return window_ ? window_->destroy() : ice::errc::not_initialized;
}

ice::error window::set(ice::window::mode mode) noexcept
{
  return window_ ? window_->set(mode) : ice::errc::not_initialized;
}

ice::error window::set(ice::window::style style) noexcept
{
  return window_ ? window_->set(style) : ice::errc::not_initialized;
}

ice::error window::text(std::string_view text) noexcept
{
  return window_ ? window_->text(text) : ice::errc::not_initialized;
}

ice::error window::icon(std::string_view icon) noexcept
{
  return window_ ? window_->icon(icon) : ice::errc::not_initialized;
}

ice::error window::start(std::string_view name, int size, int weight, ice::ui::font::flags flags) noexcept
{
  return window_ ? window_->start(name, size, weight, flags) : ice::errc::not_initialized;
}

ice::error window::show() noexcept
{
  return window_ ? window_->show() : ice::errc::not_initialized;
}

ice::error window::hide() noexcept
{
  return window_ ? window_->hide() : ice::errc::not_initialized;
}

ice::error window::minimize() noexcept
{
  return window_ ? window_->minimize() : ice::errc::not_initialized;
}

ice::error window::maximize() noexcept
{
  return window_ ? window_->maximize() : ice::errc::not_initialized;
}

ice::error window::restore() noexcept
{
  return window_ ? window_->restore() : ice::errc::not_initialized;
}

ice::error window::run() noexcept
{
  return window_ ? window_->run() : ice::errc::not_initialized;
}

void window::on_create() noexcept
{
  if (window_) {
    window_->show();
  }
}

void window::on_render(ice::ui::context& context) noexcept
{}

void window::on_render(ice::vk::context& context) noexcept
{}

void window::on_destroy() noexcept
{}

void window::on_close() noexcept
{
  if (window_) {
    window_->destroy();
  }
}

}  // namespace ice
