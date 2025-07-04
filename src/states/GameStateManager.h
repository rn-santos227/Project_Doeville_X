#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include "GameState.h"

#include <stack>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "entities/EntitiesManager.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/objects_manager/ObjectsManager.h"
namespace Project::States {
  class GameStateManager {
  public:
    GameStateManager(size_t cacheLimit, Project::Utilities::LogsManager& logsManager);
    ~GameStateManager() = default;

    void addState(const std::string& name, std::unique_ptr<GameState> state);
    void changeState(const std::string& name);
    void setInitialState(const std::string& name);
    
    void pushState(const std::string& name);
    void popState();
    
    void update(float deltaTime);
    void render();

    void enableStates(const std::vector<std::string>& names);
    void disableStates(const std::vector<std::string>& names);
    
    void cleanup();
    void cleanupCache();

    std::shared_ptr<Project::Entities::EntitiesManager> getGlobalEntitiesManager() const {
      return globalEntitiesManager;
    }

  private:
    Project::Utilities::LogsManager& logsManager;
    Project::Utilities::ObjectsManager<GameState, std::unique_ptr<GameState>> stateManager;
    size_t cacheLimit;

    std::mutex gameStateMutex;
    std::list<std::pair<std::string, std::unique_ptr<GameState>>> stateCache;
    std::unordered_map<std::string, decltype(stateCache.begin())> cacheMap;

    std::shared_ptr<Project::Entities::EntitiesManager> globalEntitiesManager = std::make_shared<Project::Entities::EntitiesManager>();
    std::stack<GameState*> stateStack;

    GameState* retrieveFromCache(const std::string& name);
    void addToCache(const std::string& name, std::unique_ptr<GameState> state);
  };
}
#endif
