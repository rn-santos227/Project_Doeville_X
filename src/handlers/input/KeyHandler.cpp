#include "KeyHandler.h"

namespace Project::Handlers {
  KeyHandler::KeyHandler(Project::Utilities::LogsManager& logsManager) : logsManager(logsManager) {
    keyBindings[KeyAction::UP] = SDL_SCANCODE_W;
    keyBindings[KeyAction::DOWN] = SDL_SCANCODE_S;
    keyBindings[KeyAction::LEFT] = SDL_SCANCODE_A;
    keyBindings[KeyAction::RIGHT] = SDL_SCANCODE_D;
    keyBindings[KeyAction::ESCAPE] = SDL_SCANCODE_ESCAPE;
    keyBindings[KeyAction::ENTER] = SDL_SCANCODE_RETURN;
    keyBindings[KeyAction::ACTION_1] = SDL_SCANCODE_Z;
    keyBindings[KeyAction::ACTION_2] = SDL_SCANCODE_Y;

    bindFunctionKeys();
    isFrozen = false;
    isDebugMode = false;
  }

  KeyHandler::~KeyHandler() {}

  void KeyHandler::bindFunctionKeys() {
    functionKeyActions[SDL_SCANCODE_F1] = [this]() { helpToggle(); };
    functionKeyActions[SDL_SCANCODE_F2] = [this]() { debugToggle(); };
    functionKeyActions[SDL_SCANCODE_F3] = [this]() { freezeGame(); }; 
    functionKeyActions[SDL_SCANCODE_F4] = [this]() { restartGame(); };
    functionKeyActions[SDL_SCANCODE_F5] = [this]() { immediateExit(); };
  }

  void KeyHandler::handleInput(SDL_Event& event) {
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

  bool KeyHandler::isKeyPressed(SDL_Scancode key) const {
    return std::find(keyPressed.begin(), keyPressed.end(), key) != keyPressed.end();
  }

  bool KeyHandler::isActionTriggered(KeyAction action) const {
    SDL_Scancode boundKey = keyBindings.at(action);
    return isKeyPressed(boundKey);
  }

  void KeyHandler::setKeyBinding(KeyAction action, SDL_Scancode key) {
    keyBindings[action] = key;
  }

  void KeyHandler::setKeyPressed(SDL_Scancode key) {
    keyPressed.push_back(key);
  }

  void KeyHandler::setKeyReleased(SDL_Scancode key) {
    keyPressed.erase(std::remove(keyPressed.begin(), keyPressed.end(), key), keyPressed.end());
  }

  void KeyHandler::helpToggle() {
    std::cout << "Help toggled." << std::endl;
  }

  void KeyHandler::debugToggle() {
    isDebugMode = !isDebugMode;
    logsManager.logMessage(isDebugMode ? "Debug Mode On." : "Debug Mode Off.");
  }

  void KeyHandler::freezeGame() {
    isFrozen = !isFrozen;
    logsManager.logMessage(isFrozen ? "Game Paused." : "Game Resumed.");
  }

  void KeyHandler::restartGame() {
    logsManager.logMessage("Game restarted.");
  }

  void KeyHandler::immediateExit() {
    logsManager.logMessage("Exiting immediately.");
    logsManager.flushLogs();
    SDL_Quit();
    exit(0);
  }
}