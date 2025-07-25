#ifndef COMPONENT_POOL_H
#define COMPONENT_POOL_H

#include <functional>
#include <memory>
#include <vector>

namespace Project::Helpers {
  template <typename T>
  class ComponentPool {
  public:

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
