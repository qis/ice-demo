#include "task.hpp"
#include <ice/format.hpp>
#include <cstdio>
#include <cstdlib>

namespace ice {

void task::promise_type::return_error(ice::error error) noexcept
{
  fmt::print(stderr, "unhandled {} error: {}\n", error.type(), error);
  std::fflush(stderr);
  {
    // TODO: Print stack trace.
  }
  std::abort();
}

}  // namespace ice
