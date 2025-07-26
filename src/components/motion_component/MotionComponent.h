#ifndef MOTION_COMPONENT_H
#define MOTION_COMPONENT_H

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/keys_component/KeysComponent.h"
#include "handlers/input/KeyHandler.h"
#include "libraries/constants/Constants.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class MotionComponent : public BaseComponent {
  public:
    MotionComponent(Project::Utilities::LogsManager& logsManager, Project::Handlers::KeyHandler* keyHandler, float speed = Project::Libraries::Constants::DEFAULT_MOTION_SPEED);
    ~MotionComponent() override = default;

    Project::Entities::Entity* getOwner() const override { return owner; }

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

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
    
    float getCurrentSpeed() const;
    void brake();

  private:
    Project::Handlers::KeyHandler* keyHandler = nullptr;
    Project::Entities::Entity* owner = nullptr;
    KeysComponent* keysComp = nullptr;

    float maxSpeed;
    float acceleration = Project::Libraries::Constants::DEFAULT_ACCELERATION;
    float brakePower = Project::Libraries::Constants::DEFAULT_BRAKE_POWER;
    float friction = Project::Libraries::Constants::DEFAULT_FRICTION;
    float velocityX = 0.0f;
    float velocityY = 0.0f;

    bool accelerationEnabled = false;
    bool rotationEnabled = false;
  };
}

#endif
