#pragma once
#include "error_code.hpp"
#include <ice/error.hpp>

constexpr ice::error u16_error_success = u16_error_code_success;
constexpr ice::error u16_error_failure = u16_error_code_failure;

constexpr ice::error i32_error_success = i32_error_code_success;
constexpr ice::error i32_error_failure = i32_error_code_failure;
