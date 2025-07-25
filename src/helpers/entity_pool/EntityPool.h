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

    template <typename... Args>
    std::unique_ptr<Project::Entities::Entity, Deleter> acquire(Args&&... args) {
      Project::Entities::Entity* mem = nullptr;
      if (!pool.empty()) {
        mem = pool.back();
        pool.pop_back();
      } else {
        mem = static_cast<Project::Entities::Entity*>(::operator new(sizeof(Project::Entities::Entity)));
      }

      new (mem) Project::Entities::Entity(std::forward<Args>(args)...);
      return std::unique_ptr<Project::Entities::Entity, Deleter>(mem, [](Project::Entities::Entity* e) {
        EntityPool::getInstance().release(e);
      });
    }

    void release(Project::Entities::Entity* entity) {
      if (!entity) return;
      entity->~Entity();
      pool.push_back(entity);
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
