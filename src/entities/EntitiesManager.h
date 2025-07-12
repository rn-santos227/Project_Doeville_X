#ifndef ENTITIES_MANAGER_H
#define ENTITIES_MANAGER_H

#include "Entity.h"

#include <lua.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

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
      std::shared_ptr<Entity> getEntity(const std::string& id);
      const std::unordered_map<std::string, std::shared_ptr<Entity>>& getAllEntities() const { return objects; }

      void unloadSceneEntities();
      void optimizeEntities();

      void initialize();
      void update(float deltaTime);
      void render();
      void reset();

      void registerEntityLuaFunctions(Entity* entity);
      
      static int lua_getEntitySpeed(lua_State* L);
      static int lua_setEntityText(lua_State* L);

    private:
      std::unordered_map<std::string, std::shared_ptr<Entity>> cachedEntities;
      bool initialized = false;
  };
}

#endif
