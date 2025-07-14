#include "MotionComponent.h"

#include <cmath>

#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/keys_component/KeysComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "libraries/categories/Categories.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/geometry/GeometryUtils.h"
#include "utilities/physics/PhysicsUtils.h"

namespace Project::Components {
  using Project::Handlers::KeyAction;
  using Project::Handlers::KeyHandler;
  using Project::Entities::Entity;

  namespace Components = Project::Libraries::Categories::Components;
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  MotionComponent::MotionComponent(Project::Utilities::LogsManager& logsManager, KeyHandler* keyHandler, float speed)
    : BaseComponent(logsManager), keyHandler(keyHandler), maxSpeed(speed) {}

  void MotionComponent::update(float deltaTime) {
    if (!owner) return;

    auto* physics = dynamic_cast<PhysicsComponent*>(owner->getComponent(Components::PHYSICS_COMPONENT));
    if (physics && rotationEnabled) physics->setRotationEnabled(true);
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

    if (!physics && (dx != 0.0f || dy != 0.0f)) {
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
    }

    if (physics) {
      physics->setVelocity(localVelX, localVelY);
    } else {
      velocityX = localVelX;
      velocityY = localVelY;
    }
  }

  void MotionComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    maxSpeed = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::SPEED, Constants::DEFAULT_MOTION_SPEED));

    bool useAccel = luaStateWrapper.getTableBoolean(tableName, Keys::USE_ACCELERATION, false);
    setAccelerationEnabled(useAccel);

    float accel = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::ACCELERATION, Constants::DEFAULT_ACCELERATION));
    setAcceleration(accel);

    float fric = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::FRICTION, Constants::DEFAULT_FRICTION));
    setFriction(fric);

    bool rotate = luaStateWrapper.getTableBoolean(tableName, Keys::ROTATION, false);
    setRotationEnabled(rotate);

    bool active = luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true);
    setActive(active);
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
