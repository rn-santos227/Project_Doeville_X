#include "MotionComponent.h"

#include <cmath>

#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "utilities/physics/PhysicsUtils.h"

#include "components/keys_component/KeysComponent.h"
#include "components/physics_component/PhysicsComponent.h"

namespace Project::Components {
  using Project::Handlers::KeyAction;
  using Project::Handlers::KeyHandler;
  using Project::Entities::Entity;

  MotionComponent::MotionComponent(Project::Utilities::LogsManager& logsManager, KeyHandler* keyHandler, float speed)
    : BaseComponent(logsManager), keyHandler(keyHandler), maxSpeed(speed) {}

  void MotionComponent::update(float deltaTime) {
    if (!owner) return;

    auto* physics = dynamic_cast<PhysicsComponent*>(owner->getComponent("PhysicsComponent"));
    float localVelX = physics ? physics->getVelocityX() : velocityX;
    float localVelY = physics ? physics->getVelocityY() : velocityY;

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
        localVelX -= acceleration * deltaTime;
        if (localVelX < -maxSpeed) localVelX = -maxSpeed;
      } else if (keys->isActionTriggered(KeyAction::MOVE_RIGHT)) {
        localVelX += acceleration * deltaTime;
        if (localVelX > maxSpeed) localVelX = maxSpeed;
      } else {
        if (localVelX > 0.0f) {
          localVelX -= friction * deltaTime;
          if (localVelX < 0.0f) localVelX = 0.0f;
        } else if (localVelX < 0.0f) {
          localVelX += friction * deltaTime;
          if (localVelX > 0.0f) localVelX = 0.0f;
        }
      }

      if (keys->isActionTriggered(KeyAction::MOVE_UP)) {
        localVelY -= acceleration * deltaTime;
        if (localVelY < -maxSpeed) localVelY = -maxSpeed;
      } else if (keys->isActionTriggered(KeyAction::MOVE_DOWN)) {
        localVelY += acceleration * deltaTime;
        if (localVelY > maxSpeed) localVelY = maxSpeed;
      } else {
        if (localVelY > 0.0f) {
          localVelY -= friction * deltaTime;
          if (localVelY < 0.0f) localVelY = 0.0f;
        } else if (localVelY < 0.0f) {
          localVelY += friction * deltaTime;
          if (localVelY > 0.0f) localVelY = 0.0f;
        }
      }

      dx = localVelX * deltaTime;
      dy = localVelY * deltaTime;
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
                float bounce = (myBox->getRestitution() + otherBox->getRestitution()) / Constants::DEFAULT_DENOMINATOR;
                float fric = (myBox->getFriction() + otherBox->getFriction()) / Constants::DEFAULT_DENOMINATOR;

                PhysicsComponent* otherPhysics = dynamic_cast<PhysicsComponent*>(entity->getComponent("PhysicsComponent"));
                if (physics && physics->getPushForce() > 0.0f && otherPhysics) {
                  float pushX = localVelX * physics->getPushForce();
                  float pushY = localVelY * physics->getPushForce();
                  otherPhysics->addVelocity(pushX, pushY);
                }

                SDL_FPoint offset{0.0f, 0.0f};
                offset = Project::Utilities::PhysicsUtils::getSnapOffset(a, b, dx, dy);

                float snapX = newX + offset.x;
                float snapY = newY + offset.y;
                owner->setPosition(snapX, snapY);

                for (const std::string& n : owner->listComponentNames()) {
                  if (auto* c = owner->getComponent(n)) {
                    if (auto* pos = dynamic_cast<PositionableComponent*>(c)) {
                      pos->setEntityPosition(static_cast<int>(snapX), static_cast<int>(snapY));
                    }
                  }
                }

                if (otherPhysics) {
                  localVelX = 0.0f;
                  localVelY = 0.0f;
                } else {
                  localVelX = -localVelX * bounce;
                  localVelY = -localVelY * bounce;
                  localVelX *= (Constants::DEFAULT_WHOLE - fric);
                  localVelY *= (Constants::DEFAULT_WHOLE - fric);
                }
                return;
              }
            }
          }
        }
      }
    }

    if (physics) {
      physics->setVelocity(localVelX, localVelY);
    } else {
      velocityX = localVelX;
      velocityY = localVelY;
    }
  }

  float MotionComponent::getCurrentSpeed() const {
    if (owner) {
      if (auto* physics = dynamic_cast<PhysicsComponent*>(owner->getComponent("PhysicsComponent"))) {
        float vx = physics->getVelocityX();
        float vy = physics->getVelocityY();
        return std::sqrt(vx * vx + vy * vy);
      }
    }
    return std::sqrt(velocityX * velocityX + velocityY * velocityY);
  }
}
