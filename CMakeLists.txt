cmake_minimum_required(VERSION 3.19 FATAL_ERROR)
project(ice VERSION 0.1.0 LANGUAGES C CXX)

# Version
configure_file(res/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/version.h LF)

# Warning Options
set(ICE_WARNING_OPTIONS)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(ICE_WARNING_OPTIONS -Wall -Wextra -Wpedantic -Werror -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable)
endif()

# Library
file(GLOB_RECURSE headers src/ice/*.hpp src/utf8proc.h src/nuklear.h)
file(GLOB_RECURSE sources src/ice/*.cpp src/utf8proc.c src/nuklear.c)

add_library(ice ${headers} ${sources})
target_compile_features(ice PUBLIC cxx_std_20)
target_compile_definitions(ice PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_compile_definitions(ice PRIVATE ICE_OS_WINDOWS_GDIPLUS=1)
target_compile_options(ice PRIVATE ${ICE_WARNING_OPTIONS})

target_include_directories(ice PRIVATE src PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>)

if(BUILD_SHARED_LIBS)
  set_target_properties(ice PROPERTIES DEFINE_SYMBOL ICE_EXPORTS)
  target_compile_definitions(ice PUBLIC ICE_SHARED)
  if(NOT WIN32)
    target_compile_options(ice PRIVATE -fvisibility=hidden)
  endif()
endif()

find_package(fmt QUIET)
if(fmt_FOUND)
  target_link_libraries(ice PUBLIC fmt::fmt)
else()
  target_include_directories(ice PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmt/include)
  target_compile_definitions(ice PUBLIC FMT_HEADER_ONLY=1)
endif()

if(WIN32)
  target_link_libraries(ice PUBLIC msimg32)
else()
  target_link_libraries(ice PUBLIC dl xcb xcb-keysyms)
endif()

install(TARGETS ice
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)

install(DIRECTORY src/ice DESTINATION include
  FILES_MATCHING PATTERN "*.hpp"
  PATTERN "src/ice/os" EXCLUDE)

# Wine
set(WINE WINEPATH=$ENV{ACE}/msvc/bin WINEDEBUG=-all wine)

# Executables
set(ICE_DISABLE_EXECUTABLES ON)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(ICE_DISABLE_EXECUTABLES OFF)
endif()

# Main
option(ICE_DISABLE_MAIN "Enable main" ${ICE_DISABLE_EXECUTABLES})
if(NOT ICE_DISABLE_MAIN)
  add_executable(main WIN32 src/main.cpp src/main.manifest)
  target_compile_definitions(main PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
  target_compile_options(main PRIVATE ${ICE_WARNING_OPTIONS})
  target_link_libraries(main PRIVATE ice)

  if(WIN32 AND CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
    add_custom_target(run-main COMMENT "Running main.exe ..."
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} USES_TERMINAL
      COMMAND ${CMAKE_COMMAND} -E env ${WINE} $<TARGET_FILE:main>)
  else()
    add_custom_target(run-main COMMENT "Running main ..."
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} USES_TERMINAL
      COMMAND $<TARGET_FILE:main>)
  endif()
endif()

# The following segment requires patched benchmark and doctest libraries.
if(ACE_TOOLCHAIN)
  # Benchmarks
  option(ICE_DISABLE_BENCHMARKS "Enable benchmarks" ${ICE_DISABLE_EXECUTABLES})
  if(NOT ICE_DISABLE_BENCHMARKS)
    add_library(symbols SHARED benchmarks/symbols.hpp benchmarks/external.cpp)
    set_target_properties(symbols PROPERTIES DEFINE_SYMBOL SYMBOLS_EXPORTS)
    target_compile_definitions(symbols PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_compile_options(symbols PRIVATE ${ICE_WARNING_OPTIONS})
    target_link_libraries(symbols PRIVATE ice)

    if(NOT WIN32)
      target_compile_options(symbols PRIVATE -fvisibility=hidden)
    endif()

    if(NOT ICE_DISABLE_MAIN)
      add_dependencies(main symbols)
    endif()

    set(benchmarks_sources benchmarks/symbols.hpp)

    if(LLVM_ENABLE_EXCEPTIONS)
      list(APPEND benchmarks_sources benchmarks/exceptions.cpp)
    else()
      list(APPEND benchmarks_sources benchmarks/code.cpp)
      list(APPEND benchmarks_sources benchmarks/error_code.cpp)
      list(APPEND benchmarks_sources benchmarks/error.cpp)
      list(APPEND benchmarks_sources benchmarks/result.cpp)
    endif()

    list(APPEND benchmarks_sources benchmarks/internal.cpp)

    add_executable(benchmarks ${benchmarks_sources} src/main.manifest)
    target_compile_definitions(benchmarks PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_compile_options(benchmarks PRIVATE ${ICE_WARNING_OPTIONS})

    find_package(benchmark REQUIRED)
    target_link_libraries(benchmarks PRIVATE ice symbols benchmark::benchmark)

    if(WIN32 AND CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
      add_custom_target(run-benchmarks COMMENT "Running benchmarks.exe ..."
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} USES_TERMINAL
        COMMAND ${CMAKE_COMMAND} -E env ${WINE} $<TARGET_FILE:benchmarks>)
    else()
      add_custom_target(run-benchmarks COMMENT "Running benchmarks ..."
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} USES_TERMINAL
        COMMAND $<TARGET_FILE:benchmarks>)
    endif()
  endif()

  # Tests
  option(ICE_DISABLE_TESTS "Enable tests" ${ICE_DISABLE_EXECUTABLES})
  if(NOT ICE_DISABLE_TESTS)
    file(GLOB_RECURSE tests_sources tests/*.[hc]pp)
    add_executable(tests ${tests_sources} src/main.manifest)
    target_compile_definitions(tests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_compile_options(tests PRIVATE ${ICE_WARNING_OPTIONS})

    find_package(doctest REQUIRED)
    target_link_libraries(tests PRIVATE ice doctest::doctest)
    doctest_discover_tests(tests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    if(WIN32 AND CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
      add_custom_target(run-tests COMMENT "Running tests.exe ..."
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} USES_TERMINAL
        COMMAND ${CMAKE_COMMAND} -E env ${WINE} $<TARGET_FILE:tests>)
    else()
      add_custom_target(run-tests COMMENT "Running tests ..."
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} USES_TERMINAL
        COMMAND $<TARGET_FILE:tests>)
    endif()
  endif()
endif()

# Alias
add_library(ice::ice ALIAS ice)
