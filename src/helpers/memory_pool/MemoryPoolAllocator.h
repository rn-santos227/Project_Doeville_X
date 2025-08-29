#ifndef MEMORY_POOL_ALLOCATOR_H
#define MEMORY_POOL_ALLOCATOR_H

#include <cstddef>
#include <vector>
#include <new>

namespace Project::Helpers {
  template <typename T, std::size_t BlockSize = 1024>
  class MemoryPoolAllocator {
  public:
    using value_type = T;

    MemoryPoolAllocator() noexcept = default;
    template <class U> constexpr MemoryPoolAllocator(const MemoryPoolAllocator<U, BlockSize>&) noexcept {}

    T* allocate(std::size_t n) {
      if (n != 1) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
      }
      if (!freeList) {
        expandPool();
      }
      Node* head = freeList;
      freeList = head->next;
      return reinterpret_cast<T*>(head);
    }

    void deallocate(T* p, std::size_t n) noexcept {
      if (!p) return;
      if (n != 1) {
        ::operator delete(p);
        return;
      }
      Node* node = reinterpret_cast<Node*>(p);
      node->next = freeList;
      freeList = node;
    }

    ~MemoryPoolAllocator() {
      for (auto& block : blocks) {
        ::operator delete(block);
      }
    }

  private:
    union Node {
      alignas(T) unsigned char storage[sizeof(T)];
      Node* next;
    };

    std::vector<Node*> blocks;
    Node* freeList = nullptr;

    void expandPool() {
      Node* block = static_cast<Node*>(::operator new(sizeof(Node) * BlockSize));
      blocks.push_back(block);
      for (std::size_t i = 0; i < BlockSize - 1; ++i) {
        block[i].next = &block[i + 1];
      }
      block[BlockSize - 1].next = nullptr;
      freeList = block;
    }
  };
}

#endif
