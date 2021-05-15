#pragma once
#include <ice/task.hpp>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <utility>

namespace ice {

// ================================================================================================
// context
// ================================================================================================

class context {
public:
  class awaitable {
    friend class context;

  public:
    awaitable() = delete;
    awaitable(awaitable&& other) = delete;
    awaitable(const awaitable& other) = delete;
    awaitable& operator=(awaitable&& other) = delete;
    awaitable& operator=(const awaitable& other) = delete;

    constexpr awaitable(context* context) noexcept
      : context_(context)
    {}

    static constexpr bool await_ready() noexcept
    {
      return false;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept
    {
      ICE_ASSERT(handle);
      ICE_ASSERT(context_);
      ICE_ASSERT(!awaiter_);
      awaiter_ = handle;
      context_->enqueue(this);
    }

    static constexpr void await_resume() noexcept
    {}

  private:
    context* context_;
    ice::coroutine_handle<> awaiter_{ nullptr };
    std::atomic<awaitable*> next_{ nullptr };
  };

  class work {
  public:
    work() = delete;

    constexpr work(work&& other) noexcept
      : context_(other.context_)
    {
      other.context_ = nullptr;
    }

    work(const work& other) = delete;

    constexpr work& operator=(work&& other) noexcept
    {
      release();
      context_ = other.context_;
      other.context_ = nullptr;
      return *this;
    }

    work& operator=(const work& other) = delete;

    work(context& context) noexcept
      : context_(std::addressof(context))
    {
      context_->size_.fetch_add(1, std::memory_order_release);
    }

    constexpr ~work()
    {
      release();
    }

    constexpr void release() noexcept
    {
      if (context_) {
        context_->size_.fetch_sub(1, std::memory_order_release);
        context_ = nullptr;
      }
    }

  private:
    context* context_{ nullptr };
  };

  context() noexcept = default;
  context(context&& other) = delete;
  context(const context& other) = delete;
  context& operator=(context&& other) = delete;
  context& operator=(const context& other) = delete;

  constexpr awaitable operator co_await() noexcept
  {
    return { this };
  }

  template <typename Callback>
  ice::task post(Callback callback) noexcept
  {
    co_await awaitable{ this };
    callback();
  }

  ICE_API ice::error run() noexcept;

  void stop() noexcept
  {
    stop_.store(true, std::memory_order_release);
    cv_.notify_all();
  }

private:
  void enqueue(awaitable* node) noexcept
  {
    ICE_ASSERT(node != nullptr);
    ICE_ASSERT(node->next_.load(std::memory_order_acquire) == nullptr);
    awaitable* tail;  // NOLINT(cppcoreguidelines-init-variables)
    awaitable* next;  // NOLINT(cppcoreguidelines-init-variables)
    size_.fetch_add(1, std::memory_order_release);
    do {
      tail = nullptr;
      next = nullptr;
      if (tail_.compare_exchange_weak(tail, node, std::memory_order_release, std::memory_order_acquire)) {
        head_.compare_exchange_strong(next, node, std::memory_order_release, std::memory_order_relaxed);
        return;
      }
    } while (!tail->next_.compare_exchange_weak(next, node, std::memory_order_release, std::memory_order_relaxed));
    tail_.compare_exchange_strong(tail, node, std::memory_order_release, std::memory_order_relaxed);
    cv_.notify_one();
  }

  awaitable* dequeue() noexcept;

  std::mutex mutex_;
  std::condition_variable cv_;
  std::atomic_size_t run_{ 0 };
  std::atomic_size_t size_{ 0 };
  std::atomic_bool stop_{ false };
  std::atomic<awaitable*> head_{ nullptr };
  std::atomic<awaitable*> tail_{ nullptr };
};

}  // namespace ice
