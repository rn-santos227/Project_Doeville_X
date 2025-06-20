#ifndef ENTITIES_MANAGER_H
#define ENTITIES_MANAGER_H

#include "Entity.h"

#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <mutex>

#include "utilities/manager/Manager.h"

using namespace Project::Utilities;

namespace Project::Entities {
  class EntitiesManager : public Manager<Entity> {
    public:
      EntitiesManager() = default;
      ~EntitiesManager() = default;

      void addEntity(const std::string& id, std::shared_ptr<Entity> entity);
      void removeEntity(const std::string& id);

      void unloadSceneEntities();
      void optimizeEntities();

      void initialize();
      void update(float deltaTime);
      void render();
      void reset();

    private:
      std::unordered_map<std::string, std::shared_ptr<Entity>> cachedEntities;
      bool initialized = false;
  };
}

#endif
