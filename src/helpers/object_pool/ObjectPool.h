#ifndef SCRIPTING_SERVICE_H
#define SCRIPTING_SERVICE_H

#include <functional>
#include <memory>
#include <vector>

namespace Project::Helpers {
  template <typename T>
  class ObjectPool {
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
  };
}

#endif
