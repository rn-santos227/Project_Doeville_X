#include "GameStateManager.h"

namespace Project::States {
  GameStateManager::GameStateManager(size_t cacheLimit)
    : cacheLimit(cacheLimit) {}

  void GameStateManager::addState(const std::string& name, std::unique_ptr<GameState> state) {
    states[name] = std::move(state);
  }

  void GameStateManager::changeState(const std::string& name) {
    if (!stateStack.empty()) {
      stateStack.top()->onExit();
      addToCache(stateStack.top()->getName(), nullptr);
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

  }

  void GameStateManager::popState() {
    if (!stateStack.empty()) {
      stateStack.top()->onExit();
      addToCache(stateStack.top()->getName(), nullptr);
      stateStack.pop();
    }

    if (!stateStack.empty()) {
      stateStack.top()->onEnter();
    }
  }

  void GameStateManager::update(float deltaTime) {
    if (!stateStack.empty()) {
      stateStack.top()->update(deltaTime);
    }
  }

  void GameStateManager::render() {
    if (!stateStack.empty()) {
      stateStack.top()->render();
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

