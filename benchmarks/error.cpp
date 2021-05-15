#include "symbols.hpp"
#include <benchmark/benchmark.h>
#include <cstdlib>

static void error_inline(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto e = symbols::error(native(success));
    benchmark::DoNotOptimize(static_cast<bool>(e));
  }
}
BENCHMARK_CAPTURE(error_inline, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(error_inline, failure, 0)->Unit(benchmark::kNanosecond);

static void error_internal(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto e = symbols::internal::error(native(success));
    benchmark::DoNotOptimize(static_cast<bool>(e));
  }
}
BENCHMARK_CAPTURE(error_internal, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(error_internal, failure, 0)->Unit(benchmark::kNanosecond);

static void error_external(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto e = symbols::external::error(native(success));
    benchmark::DoNotOptimize(static_cast<bool>(e));
  }
}
BENCHMARK_CAPTURE(error_external, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(error_external, failure, 0)->Unit(benchmark::kNanosecond);
