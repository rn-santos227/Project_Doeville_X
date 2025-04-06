#include "GameStateManager.h"

namespace Project::States {
  GameStateManager::GameStateManager(size_t cacheLimit, LogsManager& logsManager)
    : cacheLimit(cacheLimit), logsManager(logsManager) {}

  void GameStateManager::addState(const std::string& name, std::unique_ptr<GameState> state) {
    states[name] = std::move(state);
  }

  void GameStateManager::changeState(const std::string& name) {
    if (!stateStack.empty()) {
      stateStack.top()->onExit();
      addToCache(stateStack.top()->getStateName(), nullptr);
      stateStack.pop();
    }

    auto it = states.find(name);
    if (it == states.end()) {
      GameState* cachedState = retrieveFromCache(name);
      if (cachedState) {
        stateStack.push(cachedState);
        stateStack.top()->onEnter();
        return;
      }
      return;
    }

    if (!it->second->isInitialized()) {
      it->second->initialize();
      it->second->markInitialized();
    }

    stateStack.push(it->second.get());
    stateStack.top()->onEnter();
  }

  void GameStateManager::pushState(const std::string& name) {
    auto it = states.find(name);
    if (it != states.end()) {
      if (!stateStack.empty()) {
        stateStack.top()->onExit();
      }

      if (!it->second->isInitialized()) {
        it->second->initialize();
        it->second->markInitialized();
      }

      stateStack.push(it->second.get());
      stateStack.top()->onEnter();
    } else {
      GameState* cachedState = retrieveFromCache(name);
      if (cachedState) {
        stateStack.push(cachedState);
        stateStack.top()->onEnter();
      }
    }
  }

  void GameStateManager::popState() {
    if (!stateStack.empty()) {
      stateStack.top()->onExit();
      addToCache(stateStack.top()->getStateName(), nullptr);
      stateStack.pop();
    }

    if (!stateStack.empty()) {
      stateStack.top()->onEnter();
    }
  }

  void GameStateManager::update(float deltaTime) {
    std::lock_guard<std::mutex> lock(gameStateMutex);
    
    std::deque<GameState*> renderQueue;
    for (std::stack<GameState*> tempStack = stateStack; !tempStack.empty(); tempStack.pop()) {
      renderQueue.push_front(tempStack.top());
    }

    for (GameState* state : renderQueue) {
      if (state->isActive()) {
        state->update(deltaTime);
      }
    }
  }

  void GameStateManager::render(SDL_Renderer* renderer) {
    std::lock_guard<std::mutex> lock(gameStateMutex);

    std::deque<GameState*> renderQueue;
    for (std::stack<GameState*> tempStack = stateStack; !tempStack.empty(); tempStack.pop()) {
      renderQueue.push_front(tempStack.top());
    }
    
    for (GameState* state : renderQueue) {
      if (state->isActive()) {
        state->render();
      }
    }
  }

  void GameStateManager::enableStates(const std::vector<std::string>& names) {
    std::lock_guard<std::mutex> lock(gameStateMutex);
    for (const auto& name : names) {
      auto it = states.find(name);
      if (it != states.end()) {
        it->second->setActive(true);
      } else {
        logsManager.logWarning("GameState '" + name + "' not found.");
      }
    }
  }

  void GameStateManager::disableStates(const std::vector<std::string>& names) {
    std::lock_guard<std::mutex> lock(gameStateMutex);
    for (const auto& name : names) {
      auto it = states.find(name);
      if (it != states.end()) {
        it->second->setActive(false);
      } else {

      }
    }
  }

  void GameStateManager::cleanup() {
    while (!stateStack.empty()) {
      stateStack.top()->onExit();
      stateStack.pop();
    }

    states.clear();
    cleanupCache();
  }

  void GameStateManager::cleanupCache() {
    while (stateCache.size() > cacheLimit) {
      auto last = std::move(stateCache.back());
      cacheMap.erase(last.first);
      stateCache.pop_back();
    }
  }

  void GameStateManager::addToCache(const std::string& name, std::unique_ptr<GameState> state) {
    if (cacheMap.find(name) != cacheMap.end()) {
      stateCache.erase(cacheMap[name]);
    }

    GameState* rawState = state.release();
    stateCache.emplace_front(name, rawState);
    cacheMap[name] = stateCache.begin();

    cleanupCache();
  }

  GameState* GameStateManager::retrieveFromCache(const std::string& name) {
    if (cacheMap.find(name) == cacheMap.end()) {
      return nullptr;
    }

    auto it = cacheMap[name];
    stateCache.splice(stateCache.begin(), stateCache, it);
    return it->second.get();
  }
}