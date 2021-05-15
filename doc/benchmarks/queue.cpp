#include "benchmarks/symbols.hpp"
#include <wip/lockfree/queue.hpp>
#include <benchmark/benchmark.h>
#include <mutex>

struct node {
  std::atomic<node*> next{ nullptr };
};

static void enqueue(benchmark::State& state)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);

  std::mutex mutex;
  std::size_t counter = 0;
  ice::lockfree::queue<node> queue;
  std::vector<node> data{ state.max_iterations };

  for (const auto _ : state) {
#if ICE_LOCKFREE_QUEUE_ENQUEUE_SYNCHRONIZATION
    std::lock_guard lock{ mutex };
#endif
    queue.enqueue(&data[counter++]);
  }
}

static void dequeue(benchmark::State& state)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);

  struct awaitable {
    std::atomic<awaitable*> next{ nullptr };
  };

  std::mutex mutex;
  ice::lockfree::queue<node> queue;
  std::vector<node> data{ state.max_iterations };

  for (std::size_t i = 0; i < state.max_iterations; i++) {
#if ICE_LOCKFREE_QUEUE_ENQUEUE_SYNCHRONIZATION
    std::lock_guard lock{ mutex };
#endif
    queue.enqueue(&data[i]);
  }

  for (const auto _ : state) {
#if ICE_LOCKFREE_QUEUE_DEQUEUE_SYNCHRONIZATION
    std::lock_guard lock{ mutex };
#endif
    const auto node = queue.dequeue();
    benchmark::DoNotOptimize(node);
  }
}

static void dequeue_empty(benchmark::State& state)
{
  state.SetLabel(ICE_BENCHMARKS_TOOLCHAIN);

  std::mutex mutex;
  ice::lockfree::queue<node> queue;

  for (const auto _ : state) {
#if ICE_LOCKFREE_QUEUE_DEQUEUE_SYNCHRONIZATION
    std::lock_guard lock{ mutex };
#endif
    const auto node = queue.dequeue();
    benchmark::DoNotOptimize(node);
  }
}

BENCHMARK(enqueue)->Unit(benchmark::kNanosecond)->Iterations(1'000);
BENCHMARK(dequeue)->Unit(benchmark::kNanosecond)->Iterations(1'000);
BENCHMARK(enqueue)->Unit(benchmark::kNanosecond)->Iterations(1'000'000);
BENCHMARK(dequeue)->Unit(benchmark::kNanosecond)->Iterations(1'000'000);
BENCHMARK(dequeue_empty)->Unit(benchmark::kNanosecond);
