#ifndef ENTITIES_MANAGER_H
#define ENTITIES_MANAGER_H

#include "Entity.h"

#include <lua.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/SDLManager.h"
#include "helpers/objects_manager/ObjectsManager.h"
#include "interfaces/render_interface/Renderable.h"
#include "interfaces/reset_interface/Resetable.h"
#include "interfaces/update_interface/Updatable.h"
#include "libraries/constants/Constants.h"
#include "systems/motion_system/MotionSystem.h"
#include "systems/physics_system/PhysicsSystem.h"
#include "systems/render_system/RenderSystem.h"
#include "utilities/binary_cache/BinaryFileCache.h"

namespace Project::States { class GameState; }

namespace Project::Entities {
  class EntitiesManager : public Project::Helpers::ObjectsManager<Entity>, 
    public Project::Interfaces::Renderable,
    public Project::Interfaces::Resetable, 
    public Project::Interfaces::Updatable {
    public:
      EntitiesManager();
      ~EntitiesManager();

      void setGameState(Project::States::GameState* state) { gameState = state; }
      Project::States::GameState* getGameState() const { return gameState; }

      void setSDLManager(Project::Core::SDLManager* manager) { sdlManager = manager; }
      Project::Core::SDLManager* getSDLManager() const { return sdlManager; }

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

      Project::Systems::MotionSystem& getMotionSystem() { return motionSystem; }
      Project::Systems::PhysicsSystem& getPhysicsSystem() { return physicsSystem; }
      Project::Systems::RenderSystem& getRenderSystem() { return renderSystem; }

      std::vector<std::shared_ptr<Entity>> getEntitiesByGroup(const std::string& group);
      size_t getEntityCount() const;
      
      void clearGroup(const std::string& group);
      void clearScriptFunctionCache();
      void registerEntityLuaFunctions(Entity* entity);
      
    private:
      Project::Systems::MotionSystem motionSystem;
      Project::Systems::PhysicsSystem physicsSystem;
      Project::Systems::RenderSystem renderSystem;

      Project::Utilities::BinaryFileCache persistentFunctionCache;

      std::unordered_map<std::string, std::shared_ptr<Entity>> cachedEntities;
      std::unordered_map<std::string, std::vector<std::string>> entityGroups;
      std::unordered_map<std::string, std::vector<std::string>> scriptFunctionCache;
      std::unordered_map<std::string, size_t> entityIndices;
      std::unordered_map<std::string, int> idCounters;
      std::vector<std::shared_ptr<Entity>> entityList;
      
      Project::States::GameState* gameState = nullptr;
      Project::Core::SDLManager* sdlManager = nullptr;
      bool initialized = false;
  };
}

#endif
