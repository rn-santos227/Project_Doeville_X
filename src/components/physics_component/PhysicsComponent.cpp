#include "PhysicsComponent.h"

#include "components/PositionableComponent.h"
#include "entities/Entity.h"

namespace Project::Components {
  PhysicsComponent::PhysicsComponent(Project::Utilities::LogsManager& logsManager)
    : BaseComponent(logsManager) {}

  void PhysicsComponent::update(float deltaTime) {
    velocityX += accelerationX * deltaTime;
    velocityY += accelerationY * deltaTime;
  }
}
