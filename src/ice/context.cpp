#include "context.hpp"
#include <atomic>

namespace ice {

ice::error context::run() noexcept
{
  run_.fetch_add(1, std::memory_order_release);
  std::unique_lock lock{ mutex_ };
  lock.unlock();
  awaitable* node = nullptr;
  auto stop = stop_.load(std::memory_order_acquire);
  auto size = size_.load(std::memory_order_acquire);
  while (!stop && size) {
    if (node) {
      cv_.notify_one();
      ICE_ASSERT(node->awaiter_);
      node->awaiter_.resume();
      node = nullptr;
      size_.fetch_sub(1, std::memory_order_release);
    }
    lock.lock();
    cv_.wait(lock, [&]() {
      node = dequeue();
      stop = stop_.load(std::memory_order_acquire);
      size = size_.load(std::memory_order_acquire);
      return node || stop || !size;
    });
    lock.unlock();
  }
  if (node) {
    ICE_ASSERT(node->awaiter_);
    node->awaiter_.resume();
    size_.fetch_sub(1, std::memory_order_release);
  }
  run_.fetch_sub(1, std::memory_order_release);
  if (stop_.load(std::memory_order_acquire)) {
    return ice::errc::context_not_empty;
  }
  return {};
}

context::awaitable* context::dequeue() noexcept
{
  awaitable* next;  // NOLINT(cppcoreguidelines-init-variables)
  awaitable* head = head_.load(std::memory_order_acquire);
  do {
    if (head == nullptr) {
      return nullptr;
    }
    next = head->next_.load(std::memory_order_acquire);
  } while (!head_.compare_exchange_weak(head, next, std::memory_order_relaxed, std::memory_order_acquire));
  awaitable* tail = head;
  tail_.compare_exchange_strong(tail, next, std::memory_order_release, std::memory_order_relaxed);
  return head;
}

}  // namespace ice
