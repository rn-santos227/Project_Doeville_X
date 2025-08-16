#include "MotionComponent.h"
#include "MotionFunctions.h"
#include "MovementModeResolver.h"

#include <cmath>

#include "components/keys_component/KeysComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "entities/Entity.h"
#include "libraries/categories/Categories.h"
#include "libraries/constants/Constants.h"
#include "libraries/modes/MovementModes.h"
#include "libraries/keys/Keys.h"
#include "utilities/physics/PhysicsUtils.h"

namespace Project::Components {
  using Project::Handlers::KeyAction;
  using Project::Handlers::KeyHandler;
  using Project::Entities::Entity;
  using Project::Utilities::PhysicsUtils;

  namespace Components = Project::Libraries::Categories::Components;
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  MotionComponent::MotionComponent(Project::Utilities::LogsManager& logsManager, KeyHandler* keyHandler, float speed)
    : BaseComponent(logsManager), keyHandler(keyHandler) {
    maxSpeed = speed;
  }

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

    switch (movementMode) {
      case MovementMode::VEHICLE:
        MotionFunctions::handleVehicle(
          this, physics, localVelX, localVelY,
          left, right, up, down, deltaTime
        );
        break;
      
      case MovementMode::SCROLLER:
        break;
      
      case MovementMode::FLYING:
        break;
      
      case MovementMode::STANDARD:
        break;
      
      default:
        MotionFunctions::handleStandard(
          this, localVelX, localVelY,
          left, right, up, down,
          deltaTime, keys != nullptr
        );
        break;
    }

    if(accelerationEnabled) {
      Project::Utilities::Velocity temp{localVelX, localVelY};
      PhysicsUtils::clampVelocity(temp, maxSpeed);
      localVelX = temp.x;
      localVelY = temp.y;
    }

    float dx = localVelX * deltaTime;
    float dy = localVelY * deltaTime;

    if (!physics && (dx != 0.0f || dy != 0.0f)) {
      posX += dx;
      posY += dy;
      owner->setPosition(posX, posY);

      for (const std::string& name : owner->listComponentNames()) {
        if (auto* comp = owner->getComponent(name)) {
          if (auto* pos = dynamic_cast<PositionableComponent*>(comp)) {
            pos->setEntityPosition(posX, posY);
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

    std::string modeStr = luaStateWrapper.getTableString(
      tableName,
      Keys::MOVEMENT_MODE,
      Project::Libraries::Modes::Movements::STANDARD);
    setMovementMode(MovementModeResolver::resolve(modeStr));

    bool rotate = luaStateWrapper.getTableBoolean(tableName, Keys::ROTATION, false);
    setRotationEnabled(rotate);
  }

  void MotionComponent::reset() {
    data = MotionData{};
    if (owner) {
      posX = owner->getX();
      posY = owner->getY();
    }
  }

  void MotionComponent::onAttach() {
    if (owner) {
      keysComp = dynamic_cast<KeysComponent*>(owner->getComponent(Components::KEYS_COMPONENT));
      posX = owner->getX();
      posY = owner->getY();
    } else {
      keysComp = nullptr;
    }
  }

  float MotionComponent::getCurrentSpeed() const {
    return MotionFunctions::getCurrentSpeed(this);
  }

  float MotionComponent::getVelocityX() const {
    if (owner) {
      if (auto* physics = dynamic_cast<PhysicsComponent*>(owner->getComponent(Components::PHYSICS_COMPONENT))) {
        return physics->getVelocityX();
      }
    }
    return velocityX;
  }

  float MotionComponent::getVelocityY() const {
    if (owner) {
      if (auto* physics = dynamic_cast<PhysicsComponent*>(owner->getComponent(Components::PHYSICS_COMPONENT))) {
        return physics->getVelocityY();
      }
    }
    return velocityY;
  }

  void MotionComponent::brake() {
    MotionFunctions::brake(this);
  }

  void MotionComponent::turn(float speed, bool left) {
    MotionFunctions::turn(this, speed, left);
  }
}
