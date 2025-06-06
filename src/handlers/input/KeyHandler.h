#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "KeyAction.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <unordered_map> 
#include <vector>

#include "core/SDLManager.h"
#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Core;
using namespace Project::Utilities;

namespace Project::Handlers {
  class KeyHandler {
  public:
    KeyHandler(LogsManager& logsManager, SDLManager& sdlManager);
    ~KeyHandler();

    void handleInput(SDL_Event& event);
    void setKeyBinding(KeyAction action, SDL_Scancode key);
    
    bool isKeyPressed(SDL_Scancode key) const;
    bool isActionTriggered(KeyAction action) const;
    bool isGameFrozen() const { return isFrozen; }
    bool isGameDebugMode() const { return isDebugMode; }

  private:
    std::unordered_map<KeyAction, SDL_Scancode> keyBindings;
    std::unordered_map<int, std::function<void()>> functionKeyActions; 
    std::vector<SDL_Scancode> keyPressed;
    std::vector<SDL_Scancode> keyReleased;

    KeyAction currentAction;
    
    LogsManager& logsManager;
    SDLManager& sdlManager;
    
    bool isFrozen; 
    bool isDebugMode;

    void setKeyPressed(SDL_Scancode key);
    void setKeyReleased(SDL_Scancode key);

    void bindFunctionKeys();

    void helpToggle();
    void debugToggle();
    void freezeGame();
    void restartGame();
    void immediateExit();
  };
}
#endif
