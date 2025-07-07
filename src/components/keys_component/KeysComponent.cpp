#include "KeysComponent.h"

namespace Project::Components {
  using Project::Handlers::KeyAction;
  using Project::Handlers::KeyHandler;

  KeysComponent::KeysComponent(Project::Utilities::LogsManager& logsManager, KeyHandler* keyHandler)
    : BaseComponent(logsManager), keyHandler(keyHandler) {}

  void KeysComponent::update(float deltaTime) {
    if (!keyHandler || !owner) return;
  }
}
