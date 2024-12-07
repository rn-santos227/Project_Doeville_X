#include "InputHandler.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>
#include <iostream>

InputHandler::InputHandler() {
  keyBindings[InputAction::UP] = SDL_SCANCODE_W;
  keyBindings[InputAction::DOWN] = SDL_SCANCODE_S;
  keyBindings[InputAction::LEFT] = SDL_SCANCODE_A;
  keyBindings[InputAction::RIGHT] = SDL_SCANCODE_D;
  keyBindings[InputAction::ESCAPE] = SDL_SCANCODE_ESCAPE;
  keyBindings[InputAction::ENTER] = SDL_SCANCODE_RETURN;
  keyBindings[InputAction::ACTION_1] = SDL_SCANCODE_Z;
  keyBindings[InputAction::ACTION_2] = SDL_SCANCODE_Y;
}

InputHandler::~InputHandler() {}

void InputHandler::handleInput(SDL_Event& event) {

}