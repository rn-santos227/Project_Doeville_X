#include "PhysicsComponent.h"

#include "components/PositionableComponent.h"
#include "entities/Entity.h"

namespace Project::Components {
  PhysicsComponent::PhysicsComponent(Project::Utilities::LogsManager& logsManager)
    : BaseComponent(logsManager),
     weight(Project::Libraries::Constants::DEFAULT_WEIGHT),
     density(Project::Libraries::Constants::DEFAULT_DENSITY) {}

  void PhysicsComponent::update(float deltaTime) {
    velocityX += accelerationX * deltaTime;
    velocityY += accelerationY * deltaTime;
    accelerationX = 0.0f;
    accelerationY = 0.0f;

    if (friction > 0.0f) {
      float decel = friction * deltaTime;
      if (velocityX > 0.0f) {
        velocityX -= decel;
        if (velocityX < 0.0f) velocityX = 0.0f;
      } else if (velocityX < 0.0f) {
        velocityX += decel;
        if (velocityX > 0.0f) velocityX = 0.0f;
      }
      if (velocityY > 0.0f) {
        velocityY -= decel;
        if (velocityY < 0.0f) velocityY = 0.0f;
      } else if (velocityY < 0.0f) {
        velocityY += decel;
        if (velocityY > 0.0f) velocityY = 0.0f;
      }
    }

    if (density > 0.0f) {
      float factor = Constants::DEFAULT_WHOLE - density * deltaTime;
      if (factor < 0.0f) factor = 0.0f;
      velocityX *= factor;
      velocityY *= factor;
    }

    if (owner) {
      float oldX = owner->getX();
      float oldY = owner->getY();
      float newX = oldX + velocityX * deltaTime;
      float newY = oldY + velocityY * deltaTime;
      owner->setPosition(newX, newY);

      for (const std::string& name : owner->listComponentNames()) {
        if (auto* comp = owner->getComponent(name)) {
          if (auto* pos = dynamic_cast<PositionableComponent*>(comp)) {
            pos->setEntityPosition(static_cast<int>(newX), static_cast<int>(newY));
          }
        }
      }
    }
  }
}
