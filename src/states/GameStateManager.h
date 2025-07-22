#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include "GameState.h"

#include <stack>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "entities/EntitiesManager.h"
#include "interfaces/cleanup_interface/Cleanable.h"
#include "interfaces/render_interface/Renderable.h"
#include "interfaces/reset_interface/Resetable.h"
#include "interfaces/update_interface/Updatable.h"
#include "helpers/objects_manager/ObjectsManager.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::States {
  class GameStateManager : 
  public Project::Interfaces::Cleanable,
  public Project::Interfaces::Renderable,
  public Project::Interfaces::Resetable,
  public Project::Interfaces::Updatable {
  public:
    GameStateManager(size_t cacheLimit, Project::Utilities::LogsManager& logsManager);
    ~GameStateManager() = default;

    void cleanup() override;

    void addState(const std::string& name, std::unique_ptr<GameState> state);
    void changeState(const std::string& name);
    void setInitialState(const std::string& name);
    
    void pushState(const std::string& name);
    void popState();
    
    void update(float deltaTime);
    void render();
    void reset();

    void enableStates(const std::vector<std::string>& names);
    void disableStates(const std::vector<std::string>& names);
    
    void cleanupCache();
    size_t getActiveEntityCount() const;

    std::shared_ptr<Project::Entities::EntitiesManager> getGlobalEntitiesManager() const { return globalEntitiesManager; }

  private:
    Project::Helpers::ObjectsManager<GameState, std::unique_ptr<GameState>> stateManager;
    Project::Utilities::LogsManager& logsManager;
    size_t cacheLimit;

    std::mutex gameStateMutex;
    std::list<std::pair<std::string, std::unique_ptr<GameState>>> stateCache;
    std::unordered_map<std::string, decltype(stateCache.begin())> cacheMap;

    std::shared_ptr<Project::Entities::EntitiesManager> globalEntitiesManager = std::make_shared<Project::Entities::EntitiesManager>();
    std::stack<GameState*> stateStack;
    std::string initialStateName;
    
    void addToCache(const std::string& name, std::unique_ptr<GameState> state);
    GameState* retrieveFromCache(const std::string& name);
  };
}

#endif
