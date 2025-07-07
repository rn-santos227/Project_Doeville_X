#include "KeysComponent.h"

namespace Project::Components {
  using Project::Handlers::KeyAction;
  using Project::Handlers::KeyHandler;

  KeysComponent::KeysComponent(Project::Utilities::LogsManager& logsManager, KeyHandler* keyHandler)
    : BaseComponent(logsManager), keyHandler(keyHandler) {}

  void KeysComponent::update(float deltaTime) {
    if (!keyHandler || !owner) return;

    for (const auto& [action, func] : actionCallbacks) {
      if (keyHandler->isActionTriggered(action)) {
        owner->callLuaFunction(func);
      }
    }
  }

  bool KeysComponent::isActionTriggered(KeyAction action) const {
    return keyHandler ? keyHandler->isActionTriggered(action) : false;
  }

  bool KeysComponent::isKeyPressed(SDL_Scancode key) const {
    return keyHandler ? keyHandler->isKeyPressed(key) : false;
  }
}
