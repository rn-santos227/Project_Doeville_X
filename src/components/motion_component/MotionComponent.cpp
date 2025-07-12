#include "MotionComponent.h"
#include "entities/Entity.h"

#include <cmath>

#include "components/keys_component/KeysComponent.h"

namespace Project::Components {
  using Project::Handlers::KeyAction;
  using Project::Handlers::KeyHandler;
  using Project::Entities::Entity;

  MotionComponent::MotionComponent(Project::Utilities::LogsManager& logsManager, KeyHandler* keyHandler, float speed)
    : BaseComponent(logsManager), keyHandler(keyHandler), maxSpeed(speed) {}

  void MotionComponent::update(float deltaTime) {
    if (!owner) return;

    KeysComponent* keys = nullptr;
    for (const std::string& name : owner->listComponentNames()) {
      if (auto* comp = owner->getComponent(name)) {
        keys = dynamic_cast<KeysComponent*>(comp);
        if (keys) break;
      }
    }

    if (!keys) return;
    float dx = 0.0f;
    float dy = 0.0f;

    if (accelerationEnabled) {
      if (keys->isActionTriggered(KeyAction::MOVE_LEFT)) {
        velocityX -= acceleration * deltaTime;
        if (velocityX < -maxSpeed) velocityX = -maxSpeed;
      } else if (keys->isActionTriggered(KeyAction::MOVE_RIGHT)) {
        velocityX += acceleration * deltaTime;
        if (velocityX > maxSpeed) velocityX = maxSpeed;
      } else {
        velocityX = 0.0f;
      }

      if (keys->isActionTriggered(KeyAction::MOVE_UP)) {
        velocityY -= acceleration * deltaTime;
        if (velocityY < -maxSpeed) velocityY = -maxSpeed;
      } else if (keys->isActionTriggered(KeyAction::MOVE_DOWN)) {
        velocityY += acceleration * deltaTime;
        if (velocityY > maxSpeed) velocityY = maxSpeed;
      } else {
        velocityY = 0.0f;
      }

      dx = velocityX * deltaTime;
      dy = velocityY * deltaTime;
    } else {
      if (keys->isActionTriggered(KeyAction::MOVE_LEFT)) {
        dx -= maxSpeed * deltaTime;
      }
      if (keys->isActionTriggered(KeyAction::MOVE_RIGHT)) {
        dx += maxSpeed * deltaTime;
      }
      if (keys->isActionTriggered(KeyAction::MOVE_UP)) {
        dy -= maxSpeed * deltaTime;
      }
      if (keys->isActionTriggered(KeyAction::MOVE_DOWN)) {
        dy += maxSpeed * deltaTime;
      }
    }

    if (dx != 0.0f || dy != 0.0f) {
      float newX = owner->getX() + dx;
      float newY = owner->getY() + dy;
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

  float MotionComponent::getCurrentSpeed() const {
    return std::sqrt(velocityX * velocityX + velocityY * velocityY);
  }
}
