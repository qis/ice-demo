#include "symbols.hpp"
#include <benchmark/benchmark.h>
#include <cstdlib>

static void error_code_inline(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto ec = symbols::error_code(native(success));
    benchmark::DoNotOptimize(static_cast<bool>(ec));
  }
}
BENCHMARK_CAPTURE(error_code_inline, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(error_code_inline, failure, 0)->Unit(benchmark::kNanosecond);

static void error_code_internal(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto ec = symbols::internal::error_code(native(success));
    benchmark::DoNotOptimize(static_cast<bool>(ec));
  }
}
BENCHMARK_CAPTURE(error_code_internal, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(error_code_internal, failure, 0)->Unit(benchmark::kNanosecond);

static void error_code_external(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto ec = symbols::external::error_code(native(success));
    benchmark::DoNotOptimize(static_cast<bool>(ec));
  }
}
BENCHMARK_CAPTURE(error_code_external, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(error_code_external, failure, 0)->Unit(benchmark::kNanosecond);
