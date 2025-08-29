#ifndef ENTITIES_MANAGER_H
#define ENTITIES_MANAGER_H

#include "Entity.h"

#include <lua.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <SDL.h>

#include "components/BaseComponent.h"
#include "components/ComponentType.h"
#include "helpers/objects_manager/ObjectsManager.h"
#include "interfaces/render_interface/Renderable.h"
#include "interfaces/reset_interface/Resetable.h"
#include "interfaces/update_interface/Updatable.h"
#include "libraries/constants/Constants.h"
#include "platform/Platform.h"
#include "systems/behavior_system/BehaviorSystem.h"
#include "systems/motion_system/MotionSystem.h"
#include "systems/physics_system/PhysicsSystem.h"
#include "systems/render_system/RenderSystem.h"
#include "systems/system_scheduler/SystemScheduler.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/binary_cache/BinaryFileCache.h"
#include "utilities/thread/ThreadPool.h"

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

      void setPlatform(Project::Platform::Platform* platformPtr) { platform = platformPtr; }
      Project::Platform::Platform* getPlatform() const { return platform; }

      void setLogsManager(Project::Utilities::LogsManager* manager) {
        logsManager = manager;
        if (logsManager) {
          Project::Utilities::ThreadPool::getInstance().setLogger(logsManager);
        }
      }

      Project::Utilities::LogsManager* getLogsManager() const { return logsManager; }

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

      Project::Systems::BehaviorSystem& getBehaviorSystem() { return behaviorSystem; }
      Project::Systems::MotionSystem& getMotionSystem() { return motionSystem; }
      Project::Systems::PhysicsSystem& getPhysicsSystem() { return physicsSystem; }
      Project::Systems::RenderSystem& getRenderSystem() { return renderSystem; }

      std::vector<std::shared_ptr<Entity>> getEntitiesByGroup(const std::string& group);
      size_t getEntityCount() const;
      
      std::vector<Entity*> filterEntitiesByComponents(const std::vector<Project::Components::ComponentType>& types) const;
      const std::vector<Project::Components::BaseComponent*>& getComponentArray(Project::Components::ComponentType type) const;
      void serializeComponentData(const std::string& path) const;
      void deserializeComponentData(const std::string& path);

      void clearGroup(const std::string& group);
      void clearScriptFunctionCache();
      void clampEntitiesToRect(const SDL_Rect& rect);
      void registerEntityLuaFunctions(Entity* entity);
      void warpEntitiesAcrossRect(const SDL_Rect& rect);
      
    private:
      struct ComponentSoA {
        std::vector<Project::Components::BaseComponent*> components;
        std::vector<Entity*> owners;
        bool reserved = false;
        void reserve(size_t cap) {
          if (!reserved) {
            components.reserve(cap);
            owners.reserve(cap);
            reserved = true;
          }
        }
        
        void add(Project::Components::BaseComponent* comp) {
          components.push_back(comp);
          owners.push_back(comp ? comp->getOwner() : nullptr);
        }

        void remove(Project::Components::BaseComponent* comp) {
          for (size_t i = 0; i < components.size(); ++i) {
            if (components[i] == comp) {
              components[i] = components.back();
              owners[i] = owners.back();
              components.pop_back();
              owners.pop_back();
              break;
            }
          }
        }
      };

      struct Archetype {
        std::vector<Entity*> entities;
        bool reserved = false;
        void reserve(size_t cap) {
          if (!reserved) {
            entities.reserve(cap);
            reserved = true;
          }
        }

        void add(Entity* ent) { entities.push_back(ent); }

        void remove(Entity* ent) {
          for (size_t i = 0; i < entities.size(); ++i) {
            if (entities[i] == ent) {
              entities[i] = entities.back();
              entities.pop_back();
              break;
            }
          }
        }
      };

      Project::Systems::BehaviorSystem behaviorSystem;
      Project::Systems::MotionSystem motionSystem;
      Project::Systems::PhysicsSystem physicsSystem;
      Project::Systems::RenderSystem renderSystem;
      Project::Systems::SystemScheduler scheduler;

      Project::Utilities::BinaryFileCache persistentFunctionCache;
      Project::Utilities::LogsManager* logsManager = nullptr;

      std::unordered_map<Project::Components::ComponentType, ComponentSoA> componentArrays;
      std::unordered_map<std::string, std::shared_ptr<Entity>> cachedEntities;
      std::unordered_map<std::string, std::vector<std::string>> entityGroups;
      std::unordered_map<std::string, std::vector<std::string>> scriptFunctionCache;
      std::unordered_map<std::string, size_t> entityIndices;
      std::unordered_map<std::string, size_t> entityArchetypes;
      std::unordered_map<std::string, int> idCounters;
      std::vector<Archetype> archetypes;
      std::vector<size_t> archetypeKeys;
      std::unordered_map<size_t, size_t> archetypeLookup;
      std::unordered_set<std::string> disposableSeenInCamera;
      
      std::vector<std::shared_ptr<Entity>> entityList;
      std::vector<std::shared_ptr<Entity>> updateHigh;
      std::vector<std::shared_ptr<Entity>> updateNormal;
      std::vector<std::shared_ptr<Entity>> updateLow;
      std::vector<std::string> updateToRemove;
      
      Project::States::GameState* gameState = nullptr;
      Project::Platform::Platform* platform = nullptr;

      size_t updateHighCount = 0;
      size_t updateNormalCount = 0;
      size_t updateLowCount = 0;
      size_t updateToRemoveCount = 0;
      
      bool initialized = false;

      std::pair<float, float> getEntitySize(const std::shared_ptr<Entity>& entity) const;
      bool isEntityInCamera(const std::shared_ptr<Entity>& entity) const;
      bool isEntityOutOfBounds(const std::shared_ptr<Entity>& entity) const;
      void updateEntityPosition(const std::shared_ptr<Entity>& entity, float x, float y);
      void optimizeEntitiesImpl();
  };
}

#endif
