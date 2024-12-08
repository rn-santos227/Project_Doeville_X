#include "InputHandler.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>

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
  isFrozen = false;
  isDebugMode = false;
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
    setKeyPressed(key);

    if (functionKeyActions.find(key) != functionKeyActions.end()) {
      functionKeyActions[key]();
    }

  } else if (event.type == SDL_KEYUP) {
    SDL_Scancode key = event.key.keysym.scancode;
    setKeyReleased(key);
  }
}

bool InputHandler::isKeyPressed(SDL_Scancode key) const {
  return std::find(keyPressed.begin(), keyPressed.end(), key) != keyPressed.end();
}

bool InputHandler::isActionTriggered(InputAction action) const {
  SDL_Scancode boundKey = keyBindings.at(action);
  return isKeyPressed(boundKey);
}

void InputHandler::setKeyBinding(InputAction action, SDL_Scancode key) {
  keyBindings[action] = key;
}

void InputHandler::setKeyPressed(SDL_Scancode key) {
  keyPressed.push_back(key);
}

void InputHandler::setKeyReleased(SDL_Scancode key) {
  keyPressed.erase(std::remove(keyPressed.begin(), keyPressed.end(), key), keyPressed.end());
}

void InputHandler::helpToggle() {
  std::cout << "Help toggled!" << std::endl;
}

void InputHandler::debugToggle() {
  isDebugMode = !isDebugMode;
  std::cout << (isFrozen ? "Debug Mode On!" : "Debug Mode Off") << std::endl;
}

void InputHandler::freezeGame() {
  isFrozen = !isFrozen;
  std::cout << (isFrozen ? "Game frozen!" : "Game resumed!") << std::endl;
}

void InputHandler::restartGame() {
  std::cout << "Game restarted!" << std::endl;
}

void InputHandler::immediateExit() {
  std::cout << "Exiting immediately!" << std::endl;
  SDL_Quit();
  exit(0);
}