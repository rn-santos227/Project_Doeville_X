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
  DEBUG_TOGGLE,
  IMMEDIATE_EXIT,
}

#endif 