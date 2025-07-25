#ifndef COMPONENT_POOL_H
#define COMPONENT_POOL_H

#include <functional>
#include <memory>
#include <vector>

namespace Project::Helpers {
  template <typename T>
  class ComponentPool {
  public:
    using Deleter = std::function<void(T*)>;

    static ComponentPool<T>& getInstance() {
      static ComponentPool<T> instance;
      return instance;
    }

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
        ComponentPool<T>::getInstance().release(obj);
      });
    }

    void release(T* obj) {
      if (!obj) return;
      obj->~T();
      pool.push_back(obj);
    }

  private:
    std::vector<T*> pool;

    ComponentPool() = default;
    ~ComponentPool() {
      for (auto* ptr : pool) {
        ::operator delete(ptr);
      }
    }

    ComponentPool(const ComponentPool&) = delete;
    ComponentPool& operator=(const ComponentPool&) = delete;
  };
}

#endif
