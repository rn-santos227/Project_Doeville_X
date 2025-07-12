#ifndef ENTITIES_MANAGER_H
#define ENTITIES_MANAGER_H

#include "Entity.h"

#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <mutex>

#include "helpers/objects_manager/ObjectsManager.h"
#include "interfaces/render_interface/Renderable.h"
#include "interfaces/update_interface/Updatable.h"

namespace Project::Entities {
  class EntitiesManager : public Project::Helpers::ObjectsManager<Entity>, public Project::Interfaces::Renderable, public Project::Interfaces::Updatable {
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
