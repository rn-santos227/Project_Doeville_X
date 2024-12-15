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

  }

  void GameStateManager::addToCache(const std::string& name, std::unique_ptr<GameState> state) {

  }

  std::unique_ptr<GameState> GameStateManager::retrieveFromCache(const std::string& name) {

  }
}

