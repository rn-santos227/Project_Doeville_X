#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "GameState.h"

#include <string>

namespace Project::States {
  class GameState {
  public:
    virtual ~GameState() = default;
    
    virtual void initialize() = 0;

    virtual void onEnter() = 0;
    
    virtual void onExit() = 0;
    
    virtual void update(float deltaTime) = 0;
    
    virtual void render() = 0;

    virtual void handleInput() = 0;
    
    virtual std::string getName() const = 0;

    bool isInitialized() const { return initialized; }
    void markInitialized() { initialized = true; }

  protected:
    bool initialized = false;
  };
}

#endif