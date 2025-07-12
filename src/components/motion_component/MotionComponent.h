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

    void update(float deltaTime) override;
    void render() override {}

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }
    void setSpeed(float newSpeed) { maxSpeed = newSpeed; }
    float getSpeed() const { return maxSpeed; }

    void setAcceleration(float a) { acceleration = a; }
    float getAcceleration() const { return acceleration; }
    
    void setFriction(float f) { friction = f; }
    float getFriction() const { return friction; }
    
    void setAccelerationEnabled(bool enabled) { accelerationEnabled = enabled; }
    bool isAccelerationEnabled() const { return accelerationEnabled; }
    
    float getCurrentSpeed() const;

  private:
    Project::Handlers::KeyHandler* keyHandler = nullptr;
    Project::Entities::Entity* owner = nullptr;

    float maxSpeed;
    float acceleration = Project::Libraries::Constants::DEFAULT_ACCELERATION;
    float friction = Project::Libraries::Constants::DEFAULT_FRICTION;
    float velocityX = 0.0f;
    float velocityY = 0.0f;

    bool accelerationEnabled = false;
  };
}

#endif
