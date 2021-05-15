#include <wip/lockfree/queue.hpp>
#include <doctest/doctest.h>
#include <array>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

TEST_CASE("lockfree queue reuse")
{
  struct node {
    std::size_t id{ 0 };
    std::atomic<node*> next{ nullptr };
  };

  std::array<node, 1024> data;
  for (std::size_t i = 0; i < data.size(); i++) {
    data[i].id = i;
  }

  ice::lockfree::queue<node> queue;

  for (auto& node : data) {
    node.next = nullptr;
    queue.enqueue(&node);
  }
  for (auto& node : data) {
    CHECK(queue.dequeue() == &node);
  }
  CHECK(queue.dequeue() == nullptr);
  CHECK(queue.dequeue() == nullptr);

  for (auto& node : data) {
    node.next = nullptr;
    queue.enqueue(&node);
  }
  for (auto& node : data) {
    CHECK(queue.dequeue() == &node);
  }
  CHECK(queue.dequeue() == nullptr);
  CHECK(queue.dequeue() == nullptr);
}

TEST_CASE("lockfree queue single")
{
  struct node {
    std::size_t id{ 0 };
    std::atomic<node*> next{ nullptr };
  };

  node data;

  ice::lockfree::queue<node> queue;

  data.next = nullptr;
  queue.enqueue(&data);
  CHECK(queue.dequeue() == &data);

  data.next = nullptr;
  queue.enqueue(&data);
  CHECK(queue.dequeue() == &data);
  CHECK(queue.dequeue() == nullptr);

  data.next = nullptr;
  queue.enqueue(&data);
  CHECK(queue.dequeue() == &data);
  CHECK(queue.dequeue() == nullptr);
}

TEST_CASE("lockfree queue chain")
{
  struct awaitable {
    std::function<void()> callback{};
    std::atomic<awaitable*> next{ nullptr };
  };

  ice::lockfree::queue<awaitable> queue;
  std::array<awaitable, 6> data;
  std::vector<int> values;

  queue.enqueue(&data[0]);

  data[0].callback = [&]() {
    values.push_back(0);
    queue.enqueue(&data[1]);
  };

  data[1].callback = [&]() {
    values.push_back(1);
    queue.enqueue(&data[2]);
  };


  data[2].callback = [&]() {
    values.push_back(2);
    queue.enqueue(&data[3]);
  };


  data[3].callback = [&]() {
    values.push_back(3);
    queue.enqueue(&data[4]);
  };


  data[4].callback = [&]() {
    values.push_back(4);
    queue.enqueue(&data[5]);
  };

  data[5].callback = [&]() {
    values.push_back(5);
  };

  for (auto i = 0; i < 6; i++) {
    auto node = queue.dequeue();
    REQUIRE(node);
    REQUIRE(node->callback);
    node->callback();
  }

  CHECK(!queue.dequeue());
  CHECK(values.size() == 6);
  CHECK(values[0] == 0);
  CHECK(values[1] == 1);
  CHECK(values[2] == 2);
  CHECK(values[3] == 3);
  CHECK(values[4] == 4);
  CHECK(values[5] == 5);
}

TEST_CASE("lockfree queue concurrency")
{
#ifdef _MSC_VER
  constexpr std::size_t node_count = 1024 * 8;
  constexpr std::size_t consumer_thread_count = 16;
  constexpr std::size_t producer_thread_count = 64;
#else
  constexpr std::size_t node_count = 1024 * 1024;
  constexpr std::size_t consumer_thread_count = 128;
  constexpr std::size_t producer_thread_count = 512;
#endif
  constexpr std::size_t thread_count = consumer_thread_count + producer_thread_count;
  constexpr std::size_t nodes_per_producer = node_count / producer_thread_count;
  static_assert(node_count % producer_thread_count == 0);

  struct node {
    bool handled = false;
    std::atomic<node*> next{ nullptr };
  };

  ice::lockfree::queue<node> queue;
  std::vector<node> data{ node_count };

  for (std::size_t iteration = 0; iteration < 2; iteration++) {
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic_size_t nodes_handled{ 0 };
    std::atomic_size_t threads_ready{ 0 };

    const auto ready_and_wait_for_other_threads = [&]() {
      std::size_t ready = threads_ready.fetch_add(1, std::memory_order_release);
      cv.notify_all();
      while (threads_ready.load(std::memory_order_acquire) < thread_count) {
        std::unique_lock lock{ mutex };
        cv.wait(lock);
      }
      cv.notify_all();
    };

  #if ICE_LOCKFREE_QUEUE_ENQUEUE_SYNCHRONIZATION || ICE_LOCKFREE_QUEUE_DEQUEUE_SYNCHRONIZATION
    std::mutex debug_mutex;
  #endif

    std::vector<std::thread> consumers;
    consumers.reserve(consumer_thread_count);
    for (std::size_t i = 0; i < consumer_thread_count; i++) {
#if 0
      consumers.emplace_back([&]() {
        ready_and_wait_for_other_threads();
        while (nodes_handled.load(std::memory_order_acquire) < node_count) {
          node* node = nullptr;
          {
  #if ICE_LOCKFREE_QUEUE_DEQUEUE_SYNCHRONIZATION
            std::lock_guard lock{ debug_mutex };
  #endif
            node = queue.dequeue();
          }
          if (node) {
            node->handled = true;
            nodes_handled.fetch_add(1, std::memory_order_release);
          }
        }
      });
#else
      threads_ready.fetch_add(1, std::memory_order_release);
#endif
    }

    std::vector<std::thread> producers;
    producers.reserve(producer_thread_count);
    for (std::size_t i = 0; i < producer_thread_count; i++) {
      producers.emplace_back([&, index = i]() {
        const std::size_t begin = nodes_per_producer * index;
        const std::size_t end = begin + nodes_per_producer;
        ready_and_wait_for_other_threads();
        for (std::size_t i = begin; i < end; i++) {
  #if ICE_LOCKFREE_QUEUE_ENQUEUE_SYNCHRONIZATION
          std::lock_guard lock{ debug_mutex };
  #endif
          data[i].handled = false;
          data[i].next.store(nullptr, std::memory_order_release);
          queue.enqueue(&data[i]);
        }
      });
    }

    for (auto& thread : producers) {
      thread.join();
    }

    for (auto& thread : consumers) {
      thread.join();
    }

    //CHECK(queue.dequeue() == nullptr);

    std::size_t handled_count = 0;
    for (const auto& node : data) {
      if (node.handled) {
        handled_count++;
      }
    }
    //CHECK(handled_count == node_count);
  }
}
