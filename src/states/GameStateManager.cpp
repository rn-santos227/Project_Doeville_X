#include "GameStateManager.h";

namespace Project::States {
  void GameStateManager::addState(const std::string& name, std::unique_ptr<GameState> state) {
    states[name] = std::move(state);
  }
}