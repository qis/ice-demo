#include "symbols.hpp"
#include <benchmark/benchmark.h>
#include <cstdlib>

static void code_inline(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto code = symbols::code(native(success));
    benchmark::DoNotOptimize(static_cast<bool>(code));
  }
}
BENCHMARK_CAPTURE(code_inline, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(code_inline, failure, 0)->Unit(benchmark::kNanosecond);

static void code_internal(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto code = symbols::internal::code(native(success));
    benchmark::DoNotOptimize(static_cast<bool>(code));
  }
}
BENCHMARK_CAPTURE(code_internal, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(code_internal, failure, 0)->Unit(benchmark::kNanosecond);

static void code_external(benchmark::State& state, int success)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);
  native(success);
  for (const auto _ : state) {
    const auto code = symbols::external::code(native(success));
    benchmark::DoNotOptimize(static_cast<bool>(code));
  }
}
BENCHMARK_CAPTURE(code_external, success, 1)->Unit(benchmark::kNanosecond);
BENCHMARK_CAPTURE(code_external, failure, 0)->Unit(benchmark::kNanosecond);
