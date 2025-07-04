#include "KeyHandler.h"

#include "libraries/constants/Constants.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Core::SDLManager;



  KeyHandler::KeyHandler(LogsManager& logsManager, SDLManager& sdlManager)
    : logsManager(logsManager), sdlManager(sdlManager) {
    keyBindings[KeyAction::MOVE_UP] = Project::Libraries::Constants::KEY_MOVE_UP;
    keyBindings[KeyAction::MOVE_DOWN] = Project::Libraries::Constants::KEY_MOVE_DOWN;
    keyBindings[KeyAction::MOVE_LEFT] = Project::Libraries::Constants::KEY_MOVE_LEFT;
    keyBindings[KeyAction::MOVE_RIGHT] = Project::Libraries::Constants::KEY_MOVE_RIGHT;
    keyBindings[KeyAction::ESCAPE] = Project::Libraries::Constants::KEY_ESCAPE;
    keyBindings[KeyAction::ENTER] = Project::Libraries::Constants::KEY_ENTER;
    keyBindings[KeyAction::ACTION_1] = Project::Libraries::Constants::KEY_ACTION_1;
    keyBindings[KeyAction::ACTION_2] = Project::Libraries::Constants::KEY_ACTION_2;
    keyBindings[KeyAction::ACTION_3] = Project::Libraries::Constants::KEY_ACTION_3;
    keyBindings[KeyAction::ACTION_4] = Project::Libraries::Constants::KEY_ACTION_4;

    bindFunctionKeys();
    isFrozen = false;
    isDebugMode = false;
  }

  KeyHandler::~KeyHandler() {}

  void KeyHandler::bindFunctionKeys() {
    functionKeyActions[Project::Libraries::Constants::KEY_FUNC_HELP] = [this]() { helpToggle(); };
    functionKeyActions[Project::Libraries::Constants::KEY_FUNC_DEBUG] = [this]() { debugToggle(); };
    functionKeyActions[Project::Libraries::Constants::KEY_FUNC_FREEZE] = [this]() { freezeGame(); };
    functionKeyActions[Project::Libraries::Constants::KEY_FUNC_RESTART] = [this]() { restartGame(); };
    functionKeyActions[Project::Libraries::Constants::KEY_FUNC_EXIT] = [this]() { immediateExit(); };
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
