#include "symbols.hpp"
#include <ice/context.hpp>
#include <benchmark/benchmark.h>
#include <cstdlib>

static void result_void_inline(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto res = symbols::result_void(native(success));
    benchmark::DoNotOptimize(static_cast<bool>(res));
  }
}
BENCHMARK_CAPTURE(result_void_inline, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(result_void_inline, failure, 0)->Unit(benchmark::kNanosecond);

static void result_void_internal(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto res = symbols::internal::result_void(native(success));
    benchmark::DoNotOptimize(static_cast<bool>(res));
  }
}
BENCHMARK_CAPTURE(result_void_internal, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(result_void_internal, failure, 0)->Unit(benchmark::kNanosecond);

static void result_void_external(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto res = symbols::external::result_void(native(success));
    benchmark::DoNotOptimize(static_cast<bool>(res));
  }
}
BENCHMARK_CAPTURE(result_void_external, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(result_void_external, failure, 0)->Unit(benchmark::kNanosecond);

// ============================================================================

static void result_int_inline(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto res = symbols::result_int(native(success));
    if (res) {
      benchmark::DoNotOptimize(res.value());
    }
  }
}
BENCHMARK_CAPTURE(result_int_inline, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(result_int_inline, failure, 0)->Unit(benchmark::kNanosecond);

static void result_int_internal(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto res = symbols::internal::result_int(native(success));
    if (res) {
      benchmark::DoNotOptimize(res.value());
    }
  }
}
BENCHMARK_CAPTURE(result_int_internal, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(result_int_internal, failure, 0)->Unit(benchmark::kNanosecond);

static void result_int_external(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto res = symbols::external::result_int(native(success));
    if (res) {
      benchmark::DoNotOptimize(res.value());
    }
  }
}
BENCHMARK_CAPTURE(result_int_external, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(result_int_external, failure, 0)->Unit(benchmark::kNanosecond);

// ============================================================================

static void result_string_inline(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto res = symbols::result_string(native(success));
    if (res) {
      benchmark::DoNotOptimize(res.value()[0]);
    }
  }
}
BENCHMARK_CAPTURE(result_string_inline, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(result_string_inline, failure, 0)->Unit(benchmark::kNanosecond);

static void result_string_internal(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto res = symbols::internal::result_string(native(success));
    if (res) {
      benchmark::DoNotOptimize(res.value()[0]);
    }
  }
}
BENCHMARK_CAPTURE(result_string_internal, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(result_string_internal, failure, 0)->Unit(benchmark::kNanosecond);

static void result_string_external(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto res = symbols::external::result_string(native(success));
    if (res) {
      benchmark::DoNotOptimize(res.value()[0]);
    }
  }
}
BENCHMARK_CAPTURE(result_string_external, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(result_string_external, failure, 0)->Unit(benchmark::kNanosecond);

// ============================================================================

static void result_co_await(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto res = [&]() -> ice::result<void> {
      co_await symbols::result_void(native(success));
      std::abort();
      co_return {};
    }();
    const auto ok = static_cast<bool>(res);
    benchmark::DoNotOptimize(ok);
  }
}
BENCHMARK_CAPTURE(result_co_await, failure, 0)->Unit(benchmark::kNanosecond);
