# ICE
C++20 framework.

## Dependencies
Ubuntu 20.04 dependencies.

```sh
sudo apt install libxcb1-dev
```

## TODO
1. Implement Win32, Wayland, Xlib, OpenGL ES, GDI and Vulkan wrappers.
2. Implement window abstraction with Wayland, Xlib and Win32 backend.
3. Implement nuklear abstraction with OpenGL ES, GDI and Xlib backend.
4. Implement backtrace on Linux and Windows.

## Links
* <https://github.com/melak47/BorderlessWindow/blob/master/BorderlessWindow/src/BorderlessWindow.cpp>
* <https://chromium.googlesource.com/chromium/chromium/+/12a9f24150afcf2c8baec103dafe4ce76d7d64b5/ui/ozone/demo/egl_demo.cc>
* <https://github.com/SaschaWillems/Vulkan>
* <https://harrylovescode.gitbooks.io/vulkan-api/content/chap04/chap04-linux.html>
* <https://wayland.freedesktop.org/docs/html/>
* <https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Window_surface>
* <https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkWaylandSurfaceCreateInfoKHR.html>
* <https://www.khronos.org/registry/EGL/extensions/KHR/EGL_KHR_platform_wayland.txt>
* <https://lists.tizen.org/pipermail/ivi/2014-June/002695.html>
* <https://linuxhint.com/guide_linux_audio/>

## error
Custom error codes.

```cpp
#include <ice/format.hpp>

enum class errc {
  success = 0,
  failure = 1,
};

ice::error_info make_error_info(errc) {
  constexpr auto text = [](int code) -> const char* {
    switch (static_cast<errc>(code)) {
    case errc::success: return "success";
    case errc::failure: return "failure";
    }
    return nullptr;
  };
  return { "common", text };
}

int main() {
  ice::load_error_info();
  ice::load_error_info<std::errc>();
  ice::load_error_info<errc>();

  constexpr ice::error common_error = errc::failure;
  constexpr ice::error system_error = std::errc::no_such_file_or_directory;

  fmt::print("[{}] {}\n", common_error.type(), common_error);
  fmt::print("[{}] {}\n", system_error.type(), system_error);
}
```

## Wayland
```sh
sudo apt install libwayland-dev wayland-protocols
```

```cmake
if(NOT WIN32)
  target_link_libraries(ice PUBLIC wayland-client)
  set(xdg_shell_xml /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml)
  add_custom_command(COMMENT "Generating XDG shell sources ..."
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/xdg-shell.h ${CMAKE_CURRENT_BINARY_DIR}/src/xdg-shell.c
    COMMAND wayland-scanner client-header ${xdg_shell_xml} ${CMAKE_CURRENT_BINARY_DIR}/src/xdg-shell.h
    COMMAND wayland-scanner private-code  ${xdg_shell_xml} ${CMAKE_CURRENT_BINARY_DIR}/src/xdg-shell.c)
  target_sources(ice PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/src/xdg-shell.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/xdg-shell.c)
  target_link_libraries(ice PRIVATE rt)
endif()
```

## Benchmark
Benchmark results.

```
Run on (12 X 4700 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 256 KiB (x6)
  L3 Unified 12288 KiB (x1)

-------------------------------------------------------------------------
Benchmark                               Time             CPU   Iterations
-------------------------------------------------------------------------
code_inline/success                  1.28 ns         1.28 ns    543757477 LLVM
code_inline/failure                  1.29 ns         1.29 ns    513679321 LLVM
code_internal/success                1.49 ns         1.49 ns    464347323 LLVM
code_internal/failure                1.49 ns         1.49 ns    453868363 LLVM
code_external/success                2.34 ns         2.34 ns    295723651 LLVM
code_external/failure                2.34 ns         2.34 ns    296523156 LLVM
error_code_inline/success            1.49 ns         1.49 ns    465478567 LLVM
error_code_inline/failure            1.49 ns         1.49 ns    451187047 LLVM
error_code_internal/success          1.28 ns         1.28 ns    535768970 LLVM
error_code_internal/failure          1.29 ns         1.29 ns    501338387 LLVM
error_code_external/success          2.55 ns         2.55 ns    273046185 LLVM
error_code_external/failure          2.55 ns         2.55 ns    271942976 LLVM

result_void_inline/success           1.28 ns         1.28 ns    531680316 LLVM
result_void_inline/failure           1.30 ns         1.30 ns    503057790 LLVM
result_void_internal/success         1.28 ns         1.28 ns    532386416 LLVM
result_void_internal/failure         1.28 ns         1.28 ns    534237136 LLVM
result_void_external/success         2.55 ns         2.55 ns    271806370 LLVM
result_void_external/failure         2.55 ns         2.55 ns    271603699 LLVM

result_int_inline/success            1.49 ns         1.49 ns    455167388 LLVM
result_int_inline/failure            1.27 ns         1.27 ns    535368124 LLVM
result_int_internal/success          1.49 ns         1.49 ns    463768856 LLVM
result_int_internal/failure          1.49 ns         1.49 ns    465046269 LLVM
result_int_external/success          3.26 ns         3.26 ns    215759776 LLVM
result_int_external/failure          3.42 ns         3.42 ns    201936099 LLVM

result_string_inline/success         1.27 ns         1.27 ns    535720069 LLVM
result_string_inline/failure         1.27 ns         1.27 ns    523677675 LLVM
result_string_internal/success       1.27 ns         1.27 ns    522601094 LLVM
result_string_internal/failure       1.27 ns         1.27 ns    519450073 LLVM
result_string_external/success       2.76 ns         2.76 ns    251626351 LLVM
result_string_external/failure       2.76 ns         2.76 ns    251342972 LLVM

result_co_await/failure              1.28 ns         1.28 ns    546454793 LLVM

-------------------------------------------------------------------------
Benchmark                               Time             CPU   Iterations
-------------------------------------------------------------------------
exception_void_inline                1.28 ns         1.28 ns    547612060 LLVM
exception_void_internal              1.27 ns         1.27 ns    549535460 LLVM
exception_void_external              2.34 ns         2.34 ns    299355956 LLVM

exception_int_inline                 1.49 ns         1.49 ns    470527090 LLVM
exception_int_internal               1.28 ns         1.28 ns    547270874 LLVM
exception_int_external               2.66 ns         2.66 ns    265870611 LLVM

exception_string_inline              1.27 ns         1.27 ns    549313195 LLVM
exception_string_internal            1.27 ns         1.27 ns    548810505 LLVM
exception_string_external            2.83 ns         2.83 ns    245609982 LLVM

exception/failure                    2204 ns         2204 ns       316754 LLVM

-------------------------------------------------------------------------
Benchmark                               Time             CPU   Iterations
-------------------------------------------------------------------------
code_inline/success                  3.40 ns         3.40 ns    205882353 MSVC
code_inline/failure                  3.40 ns         3.40 ns    205882353 MSVC
code_internal/success                3.40 ns         3.40 ns    205882353 MSVC
code_internal/failure                3.40 ns         3.35 ns    205882353 MSVC
code_external/success                3.83 ns         3.85 ns    184210526 MSVC
code_external/failure                3.82 ns         3.80 ns    184210526 MSVC
error_code_inline/success            3.40 ns         3.40 ns    205882353 MSVC
error_code_inline/failure            3.40 ns         3.40 ns    205882353 MSVC
error_code_internal/success          3.40 ns         3.40 ns    205882353 MSVC
error_code_internal/failure          3.40 ns         3.40 ns    205882353 MSVC
error_code_external/success          4.03 ns         4.06 ns    175000000 MSVC
error_code_external/failure          4.04 ns         4.06 ns    175000000 MSVC

result_void_inline/success           3.40 ns         3.40 ns    205882353 MSVC
result_void_inline/failure           3.40 ns         3.40 ns    205882353 MSVC
result_void_internal/success         3.40 ns         3.40 ns    205882353 MSVC
result_void_internal/failure         3.40 ns         3.40 ns    205882353 MSVC
result_void_external/success         4.25 ns         4.24 ns    162790698 MSVC
result_void_external/failure         4.25 ns         4.26 ns    166666667 MSVC

result_int_inline/success            3.40 ns         3.40 ns    205882353 MSVC
result_int_inline/failure            3.44 ns         3.40 ns    205882353 MSVC
result_int_internal/success          3.40 ns         3.40 ns    205882353 MSVC
result_int_internal/failure          3.40 ns         3.40 ns    205882353 MSVC
result_int_external/success          4.25 ns         4.26 ns    166666667 MSVC
result_int_external/failure          4.04 ns         4.04 ns    170731707 MSVC

result_string_inline/success         3.40 ns         3.35 ns    205882353 MSVC
result_string_inline/failure         3.40 ns         3.40 ns    205882353 MSVC
result_string_internal/success       3.40 ns         3.40 ns    205882353 MSVC
result_string_internal/failure       3.40 ns         3.40 ns    205882353 MSVC
result_string_external/success       5.31 ns         5.31 ns    116666667 MSVC
result_string_external/failure       4.25 ns         4.24 ns    162790698 MSVC

result_co_await/failure               172 ns          172 ns      4117647 MSVC

-------------------------------------------------------------------------
Benchmark                               Time             CPU   Iterations
-------------------------------------------------------------------------
exception_void_inline                3.40 ns         3.40 ns    205882353 MSVC
exception_void_internal              3.40 ns         3.40 ns    205882353 MSVC
exception_void_external              3.82 ns         3.85 ns    184210526 MSVC

exception_int_inline                 3.40 ns         3.40 ns    205882353 MSVC
exception_int_internal               3.40 ns         3.40 ns    205882353 MSVC
exception_int_external               4.03 ns         4.04 ns    170731707 MSVC

exception_string_inline              3.40 ns         3.40 ns    205882353 MSVC
exception_string_internal            3.40 ns         3.40 ns    205882353 MSVC
exception_string_external            5.20 ns         5.21 ns    140000000 MSVC

exception/failure                    6548 ns         6500 ns       100000 MSVC
```

## Richard Hodges
I'm thinking in terms of some kind of visitation depending on platform equivalence:

```cpp
namespace ice {

enum class platform { wayland, xcb, any };

template <platform P>
struct platform_tag {
  static constexpr platform code = P;
};

bool equivalent(platform a, platform b)
{
  return a == b || a == platform::any || b == platform::any;
}

template <class F, class... Args>
bool visit(platform p, F f, Args&&... args)
{
  if constexpr (std::is_invocable_v<F, platform_tag<platform::wayland>, Args...>) {
    if (equivalent(p, platform::wayland)) {
      return f(platform_tag<platform::wayland>, std::forward<Args>(args)), true;
    }
  }
  if constexpr (std::is_invocable_v<F, platform_tag<platform::xcb>, Args...>) {
    if (equivalent(p, platform::xcb)) {
      return f(platform_tag<platform::xcb>, std::forward<Args>(args)), true;
    }
  }
  return false;
}

}  // namespace ice
```

So you could pass in a bunch of lambdas who's first argument was a `ice::platform_tag<p>` which would allow the
caller to call visit on each one until it returned true. You'd be able to write:

```cpp
struct any_window {
  template <class... Fs>
  void do_compat(Fs&&... fs)
  {
    bool done = false;
    ((done = done ? true : visit(window_type_, std::forward<Fs>(fs), this)), ...);
  }
  std::unique_ptr<window_impl_base> window_;
  ice::platform window_type_ = ice::platform::any;
};
```

## Windows
```cmd
cmake -G "Ninja Multi-Config" -DCMAKE_CONFIGURATION_TYPES="Debug;Release" -DCMAKE_TOOLCHAIN_FILE="C:/Ace/msvc.cmake" -B build
```
