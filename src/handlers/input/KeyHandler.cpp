#include "KeyHandler.h"

#include "libraries/constants/Constants.h"
#include "states/GameStateManager.h"
namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Platform::Platform;

  KeyHandler::KeyHandler(LogsManager& logsManager, Platform& platform, Project::States::GameStateManager* gameStateManager)
    : logsManager(logsManager), platform(platform), gameStateManager(gameStateManager) {
    for (int i = static_cast<int>(KeyAction::NONE);
         i <= static_cast<int>(KeyAction::IMMEDIATE_EXIT); ++i) {
      keyBindings[static_cast<KeyAction>(i)] = SDL_SCANCODE_UNKNOWN;
    }

    bindFunctionKeys();
    isFrozen = false;
    isDebugMode = false;
    keyPressed.reserve(Project::Libraries::Constants::INT_TEN);
    keyReleased.reserve(Project::Libraries::Constants::INT_TEN);
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

      if (event.key.repeat == 0) {
        auto actionIt = functionKeyActions.find(key);
        if (actionIt != functionKeyActions.end()) {
          actionIt->second();
        }
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
    if (std::find(keyPressed.begin(), keyPressed.end(), key) == keyPressed.end()) {
      keyPressed.push_back(key);
    }
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
    if (gameStateManager) {
      gameStateManager->reset();
    } else {
      logsManager.logWarning("GameStateManager not available for restart.");
    }
  }

  void KeyHandler::immediateExit() {
    if (!isDebugMode) {
      logsManager.logWarning("Immediate exit is only available in debug mode.");
      return;
    }

    if (isFrozen) {
      freezeGame();
    }

    logsManager.logMessage("Exiting immediately.");
    logsManager.flushLogs();
    platform.requestExit();
  }
}
