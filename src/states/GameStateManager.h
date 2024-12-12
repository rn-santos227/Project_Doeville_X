#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include <unordered_map>
#include <stack>
#include <memory>

#include "GameState.h"
#include "GameStateID.h"

namespace Project::States {
  class StateManager {
  public:
    StateManager() = default;
    ~StateManager() = default;

    void addState(const std::string& name, std::unique_ptr<GameState> state);
    void changeState(const std::string& name);
    void pushState(const std::string& name);
    void popState();
    void update(float deltaTime);
    void render();

  private:
    std::unordered_map<std::string, std::unique_ptr<GameState>> states;
    std::stack<GameState*> stateStack;
  };
}

#endif