#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "InputAction.h"

#include <SDL2/SDL.h>
#include <functional>
#include <unordered_map> 
#include <vector>

#include "utilities/logs_manager/LogsManager.h"

class InputHandler {
public:
  InputHandler(LogsManager& logsManager);
  ~InputHandler();

  void handleInput(SDL_Event& event);
  void setKeyBinding(InputAction action, SDL_Scancode key);
  
  bool isKeyPressed(SDL_Scancode key) const;
  bool isActionTriggered(InputAction action) const;
  bool isGameFrozen() const { return isFrozen; }
  bool isGameDebugMode() const { return isDebugMode; }

private:
  std::unordered_map<InputAction, SDL_Scancode> keyBindings;
  std::unordered_map<int, std::function<void()>> functionKeyActions; 
  std::vector<SDL_Scancode> keyPressed;
  std::vector<SDL_Scancode> keyReleased;

  InputAction currentAction;
  LogsManager& logsManager;
  
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

#endif