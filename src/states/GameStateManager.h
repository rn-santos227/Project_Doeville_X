#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include "GameState.h"
#include "GameStateID.h"

#include <unordered_map>
#include <stack>
#include <memory>

namespace Project::States {
  class GameStateManager {
  public:
    GameStateManager() = default;
    ~GameStateManager() = default;

    void addState(const std::string& name, std::unique_ptr<GameState> state);
    void changeState(const std::string& name);
    void pushState(const std::string& name);
    void popState();
    void update(float deltaTime);
    void render();
    void cleanup();

  private:
    std::unordered_map<std::string, std::unique_ptr<GameState>> states;
    std::unordered_map<std::string, std::unique_ptr<GameState>> stateCache;
    std::stack<GameState*> stateStack;
  };
}

#endif