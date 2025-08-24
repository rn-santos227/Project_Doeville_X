#include "PoolAllocator.h"

namespace Project::Utilities {
  PoolAllocator::PoolAllocator(std::size_t blockSize, std::size_t blockCount)
    : blockSize(blockSize), data(blockSize * blockCount) {
    for (std::size_t i = 0; i < blockCount; ++i) {
      freeBlocks.push(data.data() + i * blockSize);
    }
  }

  PoolAllocator::~PoolAllocator() = default;

  void* PoolAllocator::acquire() {
    std::lock_guard<std::mutex> lock(mutex);
    if (freeBlocks.empty()) {
      return nullptr;
    }
    void* ptr = freeBlocks.top();
    freeBlocks.pop();
    return ptr;
  }

  void PoolAllocator::release(void* ptr) {
    if (!ptr) return;
    std::lock_guard<std::mutex> lock(mutex);
    freeBlocks.push(ptr);
  }
}
