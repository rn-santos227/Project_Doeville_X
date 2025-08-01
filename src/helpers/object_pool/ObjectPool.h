#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <functional>
#include <memory>
#include <vector>

namespace Project::Helpers {
  template <typename T>
  class ObjectPool {
  public:
    using Deleter = std::function<void(T*)>;

    static ObjectPool<T>& getInstance() {
      static ObjectPool<T> instance;
      return instance;
    }

    void setMaxSize(size_t size) { maxSize = size; }
    size_t getMaxSize() const { return maxSize; }

    template <typename... Args>
    std::unique_ptr<T, Deleter> acquire(Args&&... args) {
      T* mem = nullptr;
      if (!pool.empty()) {
        mem = pool.back();
        pool.pop_back();
      } else {
        mem = static_cast<T*>(::operator new(sizeof(T)));
      }

      new (mem) T(std::forward<Args>(args)...);
      return std::unique_ptr<T, Deleter>(mem, [](T* obj) {
        ObjectPool<T>::getInstance().release(obj);
      });
    }

    void release(T* obj) {
      if (!obj) return;
      obj->~T();
      if (maxSize == 0 || pool.size() < maxSize) {
        pool.push_back(obj);
      } else {
        ::operator delete(obj);
      }
    }

  private:
    std::vector<T*> pool;
    size_t maxSize = 0;

    ObjectPool() = default;
    ~ObjectPool() {
      for (auto* ptr : pool) {
        ::operator delete(ptr);
      }
    }

    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
  };
}

#endif
