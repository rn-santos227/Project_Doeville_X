#ifndef LOCK_FREE_QUEUE_H
#define LOCK_FREE_QUEUE_H

#include <atomic>
#include <functional>

namespace Project::Utilities {
  template <typename T>
  class LockFreeQueue {
  private:
    struct Node {
      std::atomic<Node*> next;
      T value;
      Node(const T& v) : next(nullptr), value(v) {}
      Node() : next(nullptr), value() {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

  public:
    LockFreeQueue() {
      Node* dummy = new Node();
      head.store(dummy, std::memory_order_relaxed);
      tail.store(dummy, std::memory_order_relaxed);
    }

    ~LockFreeQueue() {
      Node* node = head.load(std::memory_order_relaxed);
      while (node) {
        Node* next = node->next.load(std::memory_order_relaxed);
        delete node;
        node = next;
      }
    }

    void push(const T& value) {
      Node* newNode = new Node(value);
      Node* oldTail = nullptr;
      
      while (true) {
        oldTail = tail.load(std::memory_order_acquire);
        Node* next = oldTail->next.load(std::memory_order_acquire);
        
        if (oldTail == tail.load(std::memory_order_acquire)) {
          if (next == nullptr) {
            if (oldTail->next.compare_exchange_weak(next, newNode, 
                                                   std::memory_order_release,
                                                   std::memory_order_relaxed)) {
              break;
            }
          } else {
            tail.compare_exchange_weak(oldTail, next, 
                                     std::memory_order_release,
                                     std::memory_order_relaxed);
          }
        }
      }
      
      tail.compare_exchange_strong(oldTail, newNode, 
                                 std::memory_order_release,
                                 std::memory_order_relaxed);
    }

    bool pop(T& result) {
      Node* oldHead = nullptr;
      Node* oldTail = nullptr;
      Node* next = nullptr;
      
      while (true) {
        oldTail = tail.load(std::memory_order_acquire);
        oldHead = head.load(std::memory_order_acquire);
        next = oldHead->next.load(std::memory_order_acquire);
        
        if (oldHead == head.load(std::memory_order_acquire)) {
          if (oldHead == oldTail) {
            if (next == nullptr) {
              return false;
            }
            tail.compare_exchange_weak(oldTail, next, 
                                     std::memory_order_release,
                                     std::memory_order_relaxed);
          } else {
            if (next == nullptr) {
              continue;
            }
            
            result = next->value;
            if (head.compare_exchange_weak(oldHead, next, 
                                         std::memory_order_release,
                                         std::memory_order_relaxed)) {
              break;
            }
          }
        }
      }
      
      if (oldHead != head.load(std::memory_order_relaxed)) {
        delete oldHead;
      }
      
      return true;
    }

    bool empty() const {
      Node* h = head.load(std::memory_order_acquire);
      Node* next = h->next.load(std::memory_order_acquire);
      return next == nullptr;
    }
  };
}
#endif
