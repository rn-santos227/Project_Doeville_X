#ifndef MOTION_COMPONENT_H
#define MOTION_COMPONENT_H

#include "MotionData.h"
#include "MovementMode.h"

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/keys_component/KeysComponent.h"
#include "handlers/input/KeyHandler.h"
#include "interfaces/reset_interface/Resetable.h"
#include "libraries/constants/Constants.h"

namespace Project { namespace Components { class MotionFunctions; } }
namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class MotionComponent : public BaseComponent, public Project::Interfaces::Resetable {
  friend class MotionFunctions;
  public:
    MotionComponent(Project::Utilities::LogsManager& logsManager, Project::Handlers::KeyHandler* keyHandler, float speed = Project::Libraries::Constants::DEFAULT_MOTION_SPEED);
    ~MotionComponent() override = default;

    Project::Entities::Entity* getOwner() const override { return owner; }
    ComponentType getType() const override { return ComponentType::MOTION; }

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void reset() override;

    void onAttach() override;
    void setKeysComponent(KeysComponent* keys) { keysComp = keys; }

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }
    void setSpeed(float newSpeed) { maxSpeed = newSpeed; }
    float getSpeed() const { return maxSpeed; }

    void setAcceleration(float _acceleration) { acceleration = _acceleration; }
    float getAcceleration() const { return acceleration; }
    
    void setFriction(float _friction) { friction = _friction; }
    float getFriction() const { return friction; }

    void setBrakePower(float _power) { brakePower = _power; }
    float getBrakePower() const { return brakePower; }
    
    void setAccelerationEnabled(bool enabled) { accelerationEnabled = enabled; }
    bool isAccelerationEnabled() const { return accelerationEnabled; }

    void setRotationEnabled(bool enabled) { rotationEnabled = enabled; }
    bool isRotationEnabled() const { return rotationEnabled; }

    void setMovementMode(MovementMode mode) { movementMode = mode; }
    MovementMode getMovementMode() const { return movementMode; }

    float getCurrentSpeed() const;
    float getVelocityX() const;
    float getVelocityY() const;
    
    void brake();
    void turn(float speed, bool clockwise);

    void setRawVelocity(float vx, float vy) { velocityX = vx; velocityY = vy; }

  private:
    Project::Handlers::KeyHandler* keyHandler = nullptr;
    Project::Entities::Entity* owner = nullptr;
    KeysComponent* keysComp = nullptr;

    MotionData data;

    float& maxSpeed = data.maxSpeed;
    float& acceleration = data.acceleration;
    float& brakePower = data.brakePower;
    float& friction = data.friction;
    float& posX = data.posX;
    float& posY = data.posY;
    float& velocityX = data.velocity.x;
    float& velocityY = data.velocity.y;
    MovementMode& movementMode = data.movementMode;

    bool& accelerationEnabled = data.accelerationEnabled;
    bool& rotationEnabled = data.rotationEnabled;
  };
}

#endif
