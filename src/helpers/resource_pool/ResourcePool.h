#ifndef RESOURCE_POOL_H
#define RESOURCE_POOL_H

#include <stack>
#include <memory>
#include <mutex>

namespace Project::Helpers {
  template<typename T>
  class ResourcePool {
  public:
    std::unique_ptr<T> acquire() {
      std::lock_guard<std::mutex> lock(mutex);
      if (pool.empty()) {
        return std::make_unique<T>();
      }
      auto ptr = std::move(pool.top());
      pool.pop();
      return ptr;
    }

    void release(std::unique_ptr<T> obj) {
      std::lock_guard<std::mutex> lock(mutex);
      pool.push(std::move(obj));
    }

  private:
    std::stack<std::unique_ptr<T>> pool;
    std::mutex mutex;
  };
}

#endif
