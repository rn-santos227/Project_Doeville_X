#include "KeyHandler.h"

#include "libraries/constants/Constants.h"

namespace Project::Handlers {
  KeyHandler::KeyHandler(LogsManager& logsManager, SDLManager& sdlManager)
    : logsManager(logsManager), sdlManager(sdlManager) {
    using namespace Project::Libraries::Constants;
    keyBindings[KeyAction::MOVE_UP] = KEY_MOVE_UP;
    keyBindings[KeyAction::MOVE_DOWN] = KEY_MOVE_DOWN;
    keyBindings[KeyAction::MOVE_LEFT] = KEY_MOVE_LEFT;
    keyBindings[KeyAction::MOVE_RIGHT] = KEY_MOVE_RIGHT;
    keyBindings[KeyAction::ESCAPE] = KEY_ESCAPE;
    keyBindings[KeyAction::ENTER] = KEY_ENTER;
    keyBindings[KeyAction::ACTION_1] = KEY_ACTION_1;
    keyBindings[KeyAction::ACTION_2] = KEY_ACTION_2;
    keyBindings[KeyAction::ACTION_3] = KEY_ACTION_3;
    keyBindings[KeyAction::ACTION_4] = KEY_ACTION_4;

    bindFunctionKeys();
    isFrozen = false;
    isDebugMode = false;
  }

  KeyHandler::~KeyHandler() {}

  void KeyHandler::bindFunctionKeys() {
    using namespace Project::Libraries::Constants;
    functionKeyActions[KEY_FUNC_HELP] = [this]() { helpToggle(); };
    functionKeyActions[KEY_FUNC_DEBUG] = [this]() { debugToggle(); };
    functionKeyActions[KEY_FUNC_FREEZE] = [this]() { freezeGame(); };
    functionKeyActions[KEY_FUNC_RESTART] = [this]() { restartGame(); };
    functionKeyActions[KEY_FUNC_EXIT] = [this]() { immediateExit(); };
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
    sdlManager.clear();
    logsManager.logMessage("Exiting immediately.");
    logsManager.flushLogs();
    exit(0);
  }
}
