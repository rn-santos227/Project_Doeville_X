#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include "GameState.h"
#include "GameStateID.h"

#include <unordered_map>
#include <stack>
#include <list>
#include <memory>
#include <mutex>

using namespace Project::Utilities;

namespace Project::States {
  class GameStateManager {
  public:
    GameStateManager(size_t cacheLimit, LogsManager& logsManager);
    ~GameStateManager() = default;

    void addState(const std::string& name, std::unique_ptr<GameState> state);
    void changeState(const std::string& name);
    
    void pushState(const std::string& name);
    void popState();
    
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);

    void enableStates(const std::vector<std::string>& names);
    void disableStates(const std::vector<std::string>& names);
    
    void cleanup();
    void cleanupCache();
    
  private:
    LogsManager& logsManager;

    size_t cacheLimit;
    std::mutex gameStateMutex;
    
    std::unordered_map<std::string, std::unique_ptr<GameState>> states;
    std::list<std::pair<std::string, std::unique_ptr<GameState>>> stateCache;
    std::unordered_map<std::string, decltype(stateCache.begin())> cacheMap;
  
    std::stack<GameState*> stateStack;

    GameState* retrieveFromCache(const std::string& name);
    void addToCache(const std::string& name, std::unique_ptr<GameState> state);
  };
}

#endif