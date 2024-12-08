#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <SDL2/SDL.h>
#include <functional>
#include <unordered_map> 
#include <vector>

enum class InputAction {
  NONE,
  UP, 
  DOWN,
  LEFT,
  RIGHT,
  ACTION_1,
  ACTION_2,
  ESCAPE,
  ENTER,
  HELP_TOGGLE,
  DEBUG_TOGGLE,
  FREEZE_GAME,
  RESTART_GAME,
  IMMEDIATE_EXIT,
};

class InputHandler {
public:
  InputHandler();
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