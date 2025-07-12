#include "MotionComponent.h"

#include <cmath>

#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "utilities/physics/PhysicsUtils.h"

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
        if (velocityX > 0.0f) {
          velocityX -= friction * deltaTime;
          if (velocityX < 0.0f) velocityX = 0.0f;
        } else if (velocityX < 0.0f) {
          velocityX += friction * deltaTime;
          if (velocityX > 0.0f) velocityX = 0.0f;
        }
      }

      if (keys->isActionTriggered(KeyAction::MOVE_UP)) {
        velocityY -= acceleration * deltaTime;
        if (velocityY < -maxSpeed) velocityY = -maxSpeed;
      } else if (keys->isActionTriggered(KeyAction::MOVE_DOWN)) {
        velocityY += acceleration * deltaTime;
        if (velocityY > maxSpeed) velocityY = maxSpeed;
      } else {
        if (velocityY > 0.0f) {
          velocityY -= friction * deltaTime;
          if (velocityY < 0.0f) velocityY = 0.0f;
        } else if (velocityY < 0.0f) {
          velocityY += friction * deltaTime;
          if (velocityY > 0.0f) velocityY = 0.0f;
        }
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
      float oldX = owner->getX();
      float oldY = owner->getY();
      float newX = oldX + dx;
      float newY = oldY + dy;
      owner->setPosition(newX, newY);

      for (const std::string& name : owner->listComponentNames()) {
        if (auto* comp = owner->getComponent(name)) {
          if (auto* pos = dynamic_cast<PositionableComponent*>(comp)) {
            pos->setEntityPosition(static_cast<int>(newX), static_cast<int>(newY));
          }
        }
      }

      auto* manager = owner->getEntitiesManager();
      auto* myBox = dynamic_cast<BoundingBoxComponent*>(owner->getComponent("BoundingBoxComponent"));
      if (manager && myBox && myBox->isSolid()) {
        for (const auto& [id, entity] : manager->getAllEntities()) {
          if (!entity || entity.get() == owner) continue;
          auto* otherBox = dynamic_cast<BoundingBoxComponent*>(entity->getComponent("BoundingBoxComponent"));
          if (!otherBox || !otherBox->isSolid()) continue;
          for (const auto& a : myBox->getBoxes()) {
            for (const auto& b : otherBox->getBoxes()) {
              if (Project::Utilities::PhysicsUtils::checkCollision(a, b)) {
                float bounce = (myBox->getRestitution() + otherBox->getRestitution()) / 2.0f;
                float fric = (myBox->getFriction() + otherBox->getFriction()) / 2.0f;
                owner->setPosition(oldX, oldY);
                for (const std::string& n : owner->listComponentNames()) {
                  if (auto* c = owner->getComponent(n)) {
                    if (auto* pos = dynamic_cast<PositionableComponent*>(c)) {
                      pos->setEntityPosition(static_cast<int>(oldX), static_cast<int>(oldY));
                    }
                  }
                }
                velocityX = -velocityX * bounce;
                velocityY = -velocityY * bounce;
                velocityX *= (1.0f - fric);
                velocityY *= (1.0f - fric);
                return;
              }
            }
          }
        }
      }
    }
  }

  float MotionComponent::getCurrentSpeed() const {
    return std::sqrt(velocityX * velocityX + velocityY * velocityY);
  }
}
