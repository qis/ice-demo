#include "symbols.hpp"
#include <cmath>

namespace symbols::external {

int code(int success) noexcept
{
  return symbols::code(success);
}

ice::error_code<std::errc> error_code(int success) noexcept
{
  return symbols::error_code(success);
}

ice::error error(int success) noexcept
{
  return symbols::error(success);
}

ice::result<void> result_void(int success) noexcept
{
  return symbols::result_void(success);
}

ice::result<int> result_int(int success) noexcept
{
  return symbols::result_int(success);
}

ice::result<string> result_string(int success) noexcept
{
  return symbols::result_string(success);
}

#if ICE_EXCEPTIONS

void exceptions_void(int success)
{
  return symbols::exceptions_void(success);
}

int exceptions_int(int success)
{
  return symbols::exceptions_int(success);
}

string exceptions_string(int success)
{
  return symbols::exceptions_string(success);
}

#endif

}  // namespace symbols::external

__attribute__((noinline)) __attribute__((optnone)) int native(int result)
{
  return result;
}
