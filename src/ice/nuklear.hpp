#pragma once
#include <ice/error.hpp>

namespace ice {

class nuklear {
public:
  struct color {
    int r{ 0 };
    int g{ 0 };
    int b{ 0 };
    int a{ 0 };
  };

  virtual ~nuklear() = default;

  virtual void layout_row_dynamic(float height, int cols) noexcept = 0;
  virtual void layout_row_static(float height, int item_width, int cols) noexcept = 0;
  virtual bool button_label(const char* text) noexcept = 0;
  virtual bool option_label(const char* text, bool active) noexcept = 0;
  virtual void property_int(const char* text, int min, int* val, int max, int step, float inc_per_pixel) noexcept = 0;
};

}  // namespace ice
