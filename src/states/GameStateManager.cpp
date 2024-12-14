#include "GameStateManager.h"

namespace Project::States {
  void GameStateManager::addState(const std::string& name, std::unique_ptr<GameState> state) {
    states[name] = std::move(state);
  }

  void GameStateManager::changeState(const std::string& name) {
    if (!stateStack.empty()) {
      stateStack.top()->onExit();
      stateStack.pop();
    }

    auto it = states.find(name);
    if (it != states.end()) {
      stateStack.push(it->second.get());
      stateStack.top()->onEnter();
    } else {
      auto stateIt = states.find(name);
      if (stateIt != states.end()) {
        stateCache[name] = std::move(stateIt->second);
        stateStack.push(stateCache[name].get());
        stateStack.top()->onEnter();
      }
    }
  }

  void GameStateManager::pushState(const std::string& name) {
    auto it = states.find(name);
    if (it != states.end()) {
      if (!stateStack.empty()) {
        stateStack.top()->onExit();
      }

      stateStack.push(it->second.get());
      stateStack.top()->onEnter();
    }
  }

  void GameStateManager::popState() {
    if (!stateStack.empty()) {
      stateStack.top()->onExit();
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
}

