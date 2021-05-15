#pragma once
#include <ice/config.hpp>
#include <atomic>
#include <concepts>
#include <type_traits>

namespace ice::lockfree {

// ================================================================================================
// queue node type
// ================================================================================================

// clang-format off

template <typename T>
concept QueueNodeType = requires(T node)
{
  std::is_class_v<T> && !std::is_const_v<T>;
  { node.next } -> std::same_as<std::atomic<T*>>;
};

// clang-format on

// ================================================================================================
// queue
// ================================================================================================
// Non-blocking, non-owning, multiple-producer, single-consumer queue based on:
// Simple, Fast, and Practical Non-Blocking and BlockingConcurrent Queue Algorithms
// by Maged M. Michae and Michael L. Scott, 1996

#define ICE_LOCKFREE_QUEUE_ENQUEUE_SYNCHRONIZATION 0
#define ICE_LOCKFREE_QUEUE_DEQUEUE_SYNCHRONIZATION 1

template <QueueNodeType T>
class queue {
public:
  using node_type = T;

  static constexpr auto acquire = std::memory_order_acquire;
  static constexpr auto release = std::memory_order_release;
  static constexpr auto relaxed = std::memory_order_relaxed;

  queue() noexcept = default;
  queue(queue&& other) noexcept = delete;
  queue(const queue& other) noexcept = delete;
  queue& operator=(queue&& other) noexcept = delete;
  queue& operator=(const queue& other) noexcept = delete;

  // Sets tail and head to node (if tail is nullptr).
  // Sets tail->next to node, then sets tail to node.
  void enqueue(node_type* node) noexcept
  {
    ICE_ASSERT(node != nullptr);
    ICE_ASSERT(node->next.load(acquire) == nullptr);
    node_type* tail;  // NOLINT(cppcoreguidelines-init-variables)
    node_type* next;  // NOLINT(cppcoreguidelines-init-variables)
    do {
      tail = nullptr;
      next = nullptr;
      if (tail_.compare_exchange_weak(tail, node, release, acquire)) {
        head_.compare_exchange_strong(next, node, release, relaxed);
        return;
      }
    } while (!tail->next.compare_exchange_weak(next, node, release, relaxed));
    tail_.compare_exchange_strong(tail, node, release, relaxed);
  }

  // Sets head to head->next (can be nullptr).
  // Sets tail to head->next (if tail points to old head).
  node_type* dequeue() noexcept
  {
    node_type* next;  // NOLINT(cppcoreguidelines-init-variables)
    node_type* head = head_.load(acquire);
    do {
      if (head == nullptr) {
        return nullptr;
      }
      next = head->next.load(acquire);
    } while (!head_.compare_exchange_weak(head, next, relaxed, acquire));
    node_type* tail = head;
    tail_.compare_exchange_strong(tail, next, release, relaxed);
    return head;
  }

private:
  std::atomic<node_type*> head_{ nullptr };
  std::atomic<node_type*> tail_{ nullptr };
};

}  // namespace ice::lockfree
