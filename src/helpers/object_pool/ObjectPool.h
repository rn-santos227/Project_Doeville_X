#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "helpers/memory_pool/MemoryPoolAllocator.h"

namespace Project::Helpers {
  template <typename T>
  class ObjectPool {
  public:
    using Deleter = std::function<void(T*)>;

    static ObjectPool<T>& getInstance() {
      static ObjectPool<T> instance;
      return instance;
    }

    void setMaxSize(size_t size) {
      std::lock_guard<std::mutex> lock(poolMutex);
      maxSize = size;
    }
    size_t getMaxSize() const {
      std::lock_guard<std::mutex> lock(poolMutex);
      return maxSize;
    }

    template <typename... Args>
    std::unique_ptr<T, Deleter> acquire(Args&&... args) {
      T* mem = nullptr;
      {
        std::lock_guard<std::mutex> lock(poolMutex);
        if (!pool.empty()) {
          mem = pool.back();
          pool.pop_back();
        } else {
          mem = allocator.allocate(1);
        }
      }

      new (mem) T(std::forward<Args>(args)...);
      return std::unique_ptr<T, Deleter>(mem, [](T* obj) {
        ObjectPool<T>::getInstance().release(obj);
      });
    }

    template <typename... Args>
    std::unique_ptr<T, Deleter> make_unique(Args&&... args) {
      return acquire(std::forward<Args>(args)...);
    }

    void release(T* obj) {
      if (!obj) return;
      obj->~T();
      std::lock_guard<std::mutex> lock(poolMutex);
      if (maxSize == 0 || pool.size() < maxSize) {
        pool.push_back(obj);
      } else {
        allocator.deallocate(obj, 1);
      }
    }

  private:
    MemoryPoolAllocator<T> allocator;

    std::vector<T*> pool;
    size_t maxSize = 0;
    mutable std::mutex poolMutex;
    
    ObjectPool() = default;
    ~ObjectPool() {
      for (auto* ptr : pool) {
        allocator.deallocate(ptr, 1);
      }
    }

    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
  };
}

#endif
