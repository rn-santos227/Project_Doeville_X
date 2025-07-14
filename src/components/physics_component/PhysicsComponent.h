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
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

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
    
    void setRotationEnabled(bool enabled) { rotationEnabled = enabled; }
    bool isRotationEnabled() const { return rotationEnabled; }
    
    void setRotation(float r) { rotation = r; }
    float getRotation() const { return rotation; }
    
    void setAngularVelocity(float av) { angularVelocity = av; }
    float getAngularVelocity() const { return angularVelocity; }

    void setAngularAcceleration(float aa) { angularAcceleration = aa; }
    float getAngularAcceleration() const { return angularAcceleration; }

    void setGravityEnabled(bool enabled) { gravityEnabled = enabled; }
    bool isGravityEnabled() const { return gravityEnabled; }

    void setGravityScale(float scale) { gravityScale = scale; }
    float getGravityScale() const { return gravityScale; }

    void setStatic(bool s) { isStatic = s; }
    bool getStatic() const { return isStatic; }

    void setKinematic(bool k) { isKinematic = k; }
    bool isKinematicBody() const { return isKinematic; }

    void setRotationSpeed(float rs) { rotationSpeed = rs; }
    float getRotationSpeed() const { return rotationSpeed; }

    void setRestitution(float r) { restitution = r; }
    float getRestitution() const { return restitution; }

    void setMass(float m) { mass = m; }
    float getMass() const { return mass; }
    
    float getWeight() const { return mass * Project::Libraries::Constants::GRAVITY; }

    void addAngularVelocity(float av) { angularVelocity += av; }
    void addAngularAcceleration(float aa) { angularAcceleration += aa; }

    void applyForce(float fx, float fy) {
      forceX += fx;
      forceY += fy;
    }

  private:
    Project::Entities::Entity* owner = nullptr;

    float forceX = 0.0f;
    float forceY = 0.0f;
    
    float accelerationX = 0.0f;
    float accelerationY = 0.0f;
    float angularAcceleration = 0.0f;
    float angularVelocity = 0.0f;
    float rotation = 0.0f;
    float velocityX = 0.0f;
    float velocityY = 0.0f;

    float friction = Project::Libraries::Constants::DEFAULT_FRICTION;
    float density = Project::Libraries::Constants::DEFAULT_DENSITY;
    float mass = Project::Libraries::Constants::DEFAULT_MASS;
    float pushForce = Project::Libraries::Constants::DEFAULT_PUSH_FORCE;
    float restitution = Project::Libraries::Constants::DEFAULT_BOUNCE_FACTOR;
    float rotationSpeed = Project::Libraries::Constants::DEFAULT_ROTATION_SPEED;
    float gravityScale = Project::Libraries::Constants::DEFAULT_GRAVITY_SCALE;

    bool rotationEnabled = false;
    bool gravityEnabled = true;
    bool isStatic = false;
    bool isKinematic = false;
  };
}

#endif
