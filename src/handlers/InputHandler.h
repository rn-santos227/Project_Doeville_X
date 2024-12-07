#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <SDL2/SDL.h>
#include <unordered_map>

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
  PAUSE_GAME,
  RESTART_GAME,
  IMMEDIATE_EXIT,
}

class InputHandler {
public:
  InputHandler();
  ~InputHandler();

  void handleInput(SDL_Event& event);
  bool isKeyPressed(SDL_Scancode key) const;
  bool isActionTriggered(InputAction action) const;

private:
  std::vector<SDL_Scancode> keyPressed;
  std::vector<SDL_Scancode> keyReleased;

  InputAction currentAction;

  void setKeyPressed(SDL_Scancode key);
  void setKeyReleased(SDL_Scancode key);
}

#endif 