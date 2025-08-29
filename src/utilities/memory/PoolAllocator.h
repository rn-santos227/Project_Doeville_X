#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include <cstddef>
#include <stack>
#include <vector>
#include <mutex>

namespace Project::Utilities {
  class PoolAllocator {
  public:
    PoolAllocator(std::size_t blockSize, std::size_t blockCount);
    ~PoolAllocator();

    void* acquire();
    void release(void* ptr);
    std::size_t getBlockSize() const { return blockSize; }

  private:
    std::size_t blockSize{0};
    std::vector<unsigned char> data;
    std::stack<void*> freeBlocks;
    std::mutex mutex;
  };
}

#endif
