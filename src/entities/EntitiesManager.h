#ifndef ENTITIES_MANAGER_H
#define ENTITIES_MANAGER_H

#include "Entity.h"

#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <mutex>

#include "utilities/objects_manager/ObjectsManager.h"

namespace Project::Entities {
  class EntitiesManager : public Project::Utilities::ObjectsManager<Entity> {
    public:
      EntitiesManager() = default;
      ~EntitiesManager() = default;

      void addEntity(const std::string& id, std::shared_ptr<Entity> entity);
      void removeEntity(const std::string& id);

      bool hasEntity(const std::string& id);

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
