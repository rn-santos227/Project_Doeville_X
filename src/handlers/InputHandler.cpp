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

  bindFunctionKeys();
}

InputHandler::~InputHandler() {}

void InputHandler::bindFunctionKeys() {
  functionKeyActions[SDL_SCANCODE_F1] = [this]() { helpToggle(); };
  functionKeyActions[SDL_SCANCODE_F2] = [this]() { debugToggle(); };
  functionKeyActions[SDL_SCANCODE_F3] = [this]() { freezeGame(); }; 
  functionKeyActions[SDL_SCANCODE_F4] = [this]() { restartGame(); };
  functionKeyActions[SDL_SCANCODE_F5] = [this]() { immediateExit(); };
}

void InputHandler::handleInput(SDL_Event& event) {
  if (event.type == SDL_KEYDOWN) {
    SDL_Scancode key = event.key.keysym.scancode;
  } else if (event.type == SDL_KEYUP) {
    SDL_Scancode key = event.key.keysym.scancode;
  }
}

bool InputHandler::isKeyPressed(SDL_Scancode key) const {
  return std::find(keyPressed.begin(), keyPressed.end(), key) != keyPressed.end();
}

bool InputHandler::isActionTriggered(InputAction action) const {
  SDL_Scancode boundKey = keyBindings.at(action);
  return isKeyPressed(boundKey);
}

void InputHandler::helpToggle() {
  std::cout << "Help toggled!" << std::endl;
}

void InputHandler::debugToggle() {
  std::cout << "Debug mode toggled!" << std::endl;
}

void InputHandler::freezeGame() {

}

void InputHandler::restartGame() {
  std::cout << "Game restarted!" << std::endl;
}

void InputHandler::immediateExit() {
  std::cout << "Exiting immediately!" << std::endl;
  SDL_Quit();
  exit(0);
}