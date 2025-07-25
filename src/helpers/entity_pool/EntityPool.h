#ifndef ENTITY_POOL_H
#define ENTITY_POOL_H

#include <functional>
#include <memory>
#include <vector>

#include "entities/Entity.h"

namespace Project::Helpers {
  class EntityPool {
  public:
    using Deleter = std::function<void(Project::Entities::Entity*)>;
    static EntityPool& getInstance() {
      static EntityPool instance;
      return instance;
    }
  
  private:
    std::vector<Project::Entities::Entity*> pool;

    EntityPool() = default;
    ~EntityPool() {
      for (auto* ptr : pool) {
        ::operator delete(ptr);
      }
    }

    EntityPool(const EntityPool&) = delete;
    EntityPool& operator=(const EntityPool&) = delete;
  };
}

#endif
