#include "symbols.hpp"
#include <benchmark/benchmark.h>
#include <exception>

#if ICE_EXCEPTIONS

static void exceptions_void_inline(benchmark::State& state)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(1);
  try {
    for (auto _ : state) {
      symbols::exceptions_void(native(1));
    }
  }
  catch (const std::exception& e) {
    benchmark::DoNotOptimize(e);
  }
}
BENCHMARK(exceptions_void_inline)->Unit(benchmark::kNanosecond);

static void exceptions_void_internal(benchmark::State& state)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(1);
  try {
    for (auto _ : state) {
      symbols::internal::exceptions_void(native(1));
    }
  }
  catch (const std::exception& e) {
    benchmark::DoNotOptimize(e);
  }
}
BENCHMARK(exceptions_void_internal)->Unit(benchmark::kNanosecond);

static void exceptions_void_external(benchmark::State& state)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(1);
  try {
    for (auto _ : state) {
      symbols::external::exceptions_void(native(1));
    }
  }
  catch (const std::exception& e) {
    benchmark::DoNotOptimize(e);
  }
}
BENCHMARK(exceptions_void_external)->Unit(benchmark::kNanosecond);

// ============================================================================

static void exceptions_int_inline(benchmark::State& state)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(1);
  try {
    for (auto _ : state) {
      const auto i = symbols::exceptions_int(native(1));
      benchmark::DoNotOptimize(i);
    }
  }
  catch (const std::exception& e) {
    benchmark::DoNotOptimize(e);
  }
}
BENCHMARK(exceptions_int_inline)->Unit(benchmark::kNanosecond);

static void exceptions_int_internal(benchmark::State& state)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(1);
  try {
    for (auto _ : state) {
      const auto i = symbols::internal::exceptions_int(native(1));
      benchmark::DoNotOptimize(i);
    }
  }
  catch (const std::exception& e) {
    benchmark::DoNotOptimize(e);
  }
}
BENCHMARK(exceptions_int_internal)->Unit(benchmark::kNanosecond);

static void exceptions_int_external(benchmark::State& state)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(1);
  try {
    for (auto _ : state) {
      const auto i = symbols::external::exceptions_int(native(1));
      benchmark::DoNotOptimize(i);
    }
  }
  catch (const std::exception& e) {
    benchmark::DoNotOptimize(e);
  }
}
BENCHMARK(exceptions_int_external)->Unit(benchmark::kNanosecond);

// ============================================================================

static void exceptions_string_inline(benchmark::State& state)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(1);
  try {
    for (auto _ : state) {
      const auto s = symbols::exceptions_string(native(1));
      benchmark::DoNotOptimize(s[0]);
    }
  }
  catch (const std::exception& e) {
    benchmark::DoNotOptimize(e);
  }
}
BENCHMARK(exceptions_string_inline)->Unit(benchmark::kNanosecond);

static void exceptions_string_internal(benchmark::State& state)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(1);
  try {
    for (auto _ : state) {
      const auto s = symbols::internal::exceptions_string(native(1));
      benchmark::DoNotOptimize(s[0]);
    }
  }
  catch (const std::exception& e) {
    benchmark::DoNotOptimize(e);
  }
}
BENCHMARK(exceptions_string_internal)->Unit(benchmark::kNanosecond);

static void exceptions_string_external(benchmark::State& state)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(1);
  try {
    for (auto _ : state) {
      const auto s = symbols::external::exceptions_string(native(1));
      benchmark::DoNotOptimize(s[0]);
    }
  }
  catch (const std::exception& e) {
    benchmark::DoNotOptimize(e);
  }
}
BENCHMARK(exceptions_string_external)->Unit(benchmark::kNanosecond);

// ============================================================================

static void exception(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (auto _ : state) {
    try {
      symbols::exceptions_void(native(success));
    }
    catch (const std::exception& e) {
      benchmark::DoNotOptimize(e);
    }
  }
}
BENCHMARK_CAPTURE(exception, failure, 0)->Unit(benchmark::kNanosecond);

#endif
