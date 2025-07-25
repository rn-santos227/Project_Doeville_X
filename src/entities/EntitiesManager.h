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
#include "interfaces/reset_interface/Resetable.h"
#include "interfaces/update_interface/Updatable.h"
#include "libraries/constants/Constants.h"

namespace Project::States { class GameState; }

namespace Project::Entities {
  class EntitiesManager : public Project::Helpers::ObjectsManager<Entity>, 
    public Project::Interfaces::Renderable,
    public Project::Interfaces::Resetable, 
    public Project::Interfaces::Updatable {
    public:
      EntitiesManager() = default;
      ~EntitiesManager() = default;

      void setGameState(Project::States::GameState* state) { gameState = state; }
      Project::States::GameState* getGameState() const { return gameState; }

      std::string addEntity(std::shared_ptr<Entity> entity, const std::string& id = Project::Libraries::Constants::EMPTY_STRING);
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

      std::vector<std::shared_ptr<Entity>> getEntitiesByGroup(const std::string& group);
      void clearGroup(const std::string& group);
      void registerEntityLuaFunctions(Entity* entity);

      size_t getEntityCount() const;

    private:
      std::unordered_map<std::string, std::shared_ptr<Entity>> cachedEntities;
      std::unordered_map<std::string, std::vector<std::string>> entityGroups;
      std::unordered_map<std::string, int> idCounters;
      std::vector<std::shared_ptr<Entity>> entityList;
      
      Project::States::GameState* gameState = nullptr;
      bool initialized = false;
  };
}

#endif
