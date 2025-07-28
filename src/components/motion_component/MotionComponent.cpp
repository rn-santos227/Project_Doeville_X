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
#include "utilities/math/MathUtils.h"
#include "utilities/physics/PhysicsUtils.h"

namespace Project::Components {
  using Project::Handlers::KeyAction;
  using Project::Handlers::KeyHandler;
  using Project::Entities::Entity;
  using Project::Utilities::MathUtils;
  using Project::Utilities::PhysicsUtils;

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

    KeysComponent* keys = keysComp;
    bool left = keys && keys->isActionTriggered(KeyAction::MOVE_LEFT);
    bool right = keys && keys->isActionTriggered(KeyAction::MOVE_RIGHT);
    bool up = keys && keys->isActionTriggered(KeyAction::MOVE_UP);
    bool down = keys && keys->isActionTriggered(KeyAction::MOVE_DOWN);

    if (accelerationEnabled) {
      if (left) {
        localVelX -= acceleration * deltaTime;
        if (localVelX < -maxSpeed) localVelX = -maxSpeed;
      } else if (right) {
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

      if (up) {
        localVelY -= acceleration * deltaTime;
        if (localVelY < -maxSpeed) localVelY = -maxSpeed;
      } else if (down) {
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

    } else {
      if (keys) {
        localVelX = 0.0f;
        localVelY = 0.0f;

        if (left) {
          localVelX = -maxSpeed;
        } else if (right) {
          localVelX = maxSpeed;
        }

        if (up) {
          localVelY = -maxSpeed;
        } else if (down) {
          localVelY = maxSpeed;
        }
      }
    }

    if(accelerationEnabled) {
      PhysicsUtils::clampVelocityInPlace(localVelX, localVelY, maxSpeed);
    }

    float dx = localVelX * deltaTime;
    float dy = localVelY * deltaTime;

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

    float bPower = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::BRAKE_POWER, Constants::DEFAULT_BRAKE_POWER));
    setBrakePower(bPower);

    bool rotate = luaStateWrapper.getTableBoolean(tableName, Keys::ROTATION, false);
    setRotationEnabled(rotate);
  }

  void MotionComponent::onAttach() {
    if (owner) {
      keysComp = dynamic_cast<KeysComponent*>(
        owner->getComponent(Components::KEYS_COMPONENT));
    } else {
      keysComp = nullptr;
    }
  }

  float MotionComponent::getCurrentSpeed() const {
    if (owner) {
      if (auto* physics = dynamic_cast<PhysicsComponent*>(owner->getComponent(Components::PHYSICS_COMPONENT))) {
        float vx = physics->getVelocityX();
        float vy = physics->getVelocityY();
        return MathUtils::magnitude(vx, vy);
      }
    }
    return MathUtils::magnitude(velocityX, velocityY);
  }

  void MotionComponent::brake() {
    if (!owner) {
      if (brakePower >= Constants::DEFAULT_WHOLE) {
        velocityX = 0.0f;
        velocityY = 0.0f;
      } else {
        velocityX *= (Constants::DEFAULT_WHOLE - brakePower);
        velocityY *= (Constants::DEFAULT_WHOLE - brakePower);
      }
      return;
    }

    if (auto* physics = dynamic_cast<PhysicsComponent*>(owner->getComponent(Components::PHYSICS_COMPONENT))) {
      if (brakePower >= Constants::DEFAULT_WHOLE) {
        physics->setVelocity(0.0f, 0.0f);
      } else {
        float newVX = physics->getVelocityX() * (Constants::DEFAULT_WHOLE - brakePower);
        float newVY = physics->getVelocityY() * (Constants::DEFAULT_WHOLE - brakePower);
        physics->setVelocity(newVX, newVY);
      }
    } else {
      if (brakePower >= Constants::DEFAULT_WHOLE) {
        velocityX = 0.0f;
        velocityY = 0.0f;
      } else {
        velocityX *= (Constants::DEFAULT_WHOLE - brakePower);
        velocityY *= (Constants::DEFAULT_WHOLE- brakePower);
      }
    }
  }

  void MotionComponent::turn(float speed, bool left) {
    if (!owner) return;
    auto* box = dynamic_cast<BoundingBoxComponent*>(owner->getComponent(Components::BOUNDING_BOX_COMPONENT));
    if (!box || !box->isRotationEnabled()) return;

    float amount = std::abs(speed);
    if (auto* physics = dynamic_cast<PhysicsComponent*>(owner->getComponent(Components::PHYSICS_COMPONENT))) {
      physics->setRotationEnabled(true);
      physics->setAngularVelocity(left ? -amount : amount);
    } else {
      box->setRotation(box->getRotation() + (left ? -amount : amount));
    }
  }
}
