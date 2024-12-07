#include "InputHandler.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <unordered_map>

InputHandler::InputHandler() : currentAction(InputAction::NONE) {}

InputHandler::~InputHandler() {}