#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "components/BaseComponent.h"
#include "libraries/constants/Constants.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class PhysicsComponent : public BaseComponent {
  public:
    explicit PhysicsComponent(Project::Utilities::LogsManager& logsManager);
    ~PhysicsComponent() override = default;

    void resolveCollisionWith(PhysicsComponent* other, float restitution);

    void update(float deltaTime) override;
    void render() override {}

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }

    void setVelocity(float vx, float vy) { velocityX = vx; velocityY = vy; }
    void addVelocity(float vx, float vy) { velocityX += vx; velocityY += vy; }
    
    float getVelocityX() const { return velocityX; }
    float getVelocityY() const { return velocityY; }

    void setAcceleration(float ax, float ay) { accelerationX = ax; accelerationY = ay; }
    void addAcceleration(float ax, float ay) { accelerationX += ax; accelerationY += ay; }
    
    float getAccelerationX() const { return accelerationX; }
    float getAccelerationY() const { return accelerationY; }

    void setFriction(float f) { friction = f; }
    float getFriction() const { return friction; }

    void setPushForce(float f) { pushForce = f; }
    float getPushForce() const { return pushForce; }

    void setDensity(float d) { density = d; }
    float getDensity() const { return density; }

    void setRestitution(float r) { restitution = r; }
    float getRestitution() const { return restitution; }

    void setWeight(float w) { weight = w; }
    float getWeight() const { return weight; }

    void applyForce(float fx, float fy) {
      accelerationX += fx / weight;
      accelerationY += fy / weight;
    }

  private:
    Project::Entities::Entity* owner = nullptr;
    
    float velocityX = 0.0f;
    float velocityY = 0.0f;
    float accelerationX = 0.0f;
    float accelerationY = 0.0f;
    float friction = Project::Libraries::Constants::DEFAULT_FRICTION;
    float density = Project::Libraries::Constants::DEFAULT_DENSITY;
    float pushForce = Project::Libraries::Constants::DEFAULT_PUSH_FORCE;
    float restitution = Project::Libraries::Constants::DEFAULT_BOUNCE_FACTOR;
    float weight = Project::Libraries::Constants::DEFAULT_WEIGHT;
  };
}

#endif
