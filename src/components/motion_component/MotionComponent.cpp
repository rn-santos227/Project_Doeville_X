#include "MotionComponent.h"

#include "components/keys_component/KeysComponent.h"

namespace Project::Components {
  using Project::Handlers::KeyAction;
  using Project::Handlers::KeyHandler;
  using Project::Entities::Entity;

  MotionComponent::MotionComponent(Project::Utilities::LogsManager& logsManager, KeyHandler* keyHandler, float speed)
    : BaseComponent(logsManager), keyHandler(keyHandler), speed(speed) {}

  void MotionComponent::update(float deltaTime) {
    if (!owner) return;
  }
}
