#include "GameStateManager.h"

namespace Project::States {
  void GameStateManager::addState(const std::string& name, std::unique_ptr<GameState> state) {
    states[name] = std::move(state);
  }

  void GameStateManager::changeState(const std::string& name) {

  }

  void GameStateManager::pushState(const std::string& name) {

  }

  void GameStateManager::popState() {

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

  }
}

