#ifndef PHYSICS_DATA_H
#define PHYSICS_DATA_H

#include "libraries/constants/Constants.h"
#include "utilities/physics/PhysicsUtils.h"

namespace Project::Components {
  enum class UpdateFrequency { HIGH, NORMAL, LOW };
  struct PhysicsData {
    UpdateFrequency updateFrequency = UpdateFrequency::NORMAL;

    float tickRate = 0.0f;
    float tickAccumulator = 0.0f;
    
    // Linear motion
    Project::Utilities::Velocity force;
    Project::Utilities::Velocity acceleration;
    Project::Utilities::Velocity velocity;

    // Angular motion
    float angularAcceleration = 0.0f;
    float angularVelocity = 0.0f;
    float rotation = 0.0f;

    // Properties
    float friction = Project::Libraries::Constants::DEFAULT_FRICTION;
    float damping = Project::Libraries::Constants::DEFAULT_DAMPING;
    float density = Project::Libraries::Constants::DEFAULT_DENSITY;
    float mass = Project::Libraries::Constants::DEFAULT_MASS;
    float pushForce = Project::Libraries::Constants::DEFAULT_PUSH_FORCE;
    float restitution = Project::Libraries::Constants::DEFAULT_BOUNCE_FACTOR;
    float rotationSpeed = Project::Libraries::Constants::DEFAULT_ROTATION_SPEED;
    float gravityScale = Project::Libraries::Constants::DEFAULT_GRAVITY_SCALE;

    // Flags
    bool rotationEnabled = false;
    bool gravityEnabled = false;
    bool isStatic = false;
    bool isKinematic = false;

    [[nodiscard]]
    float getWeight() const {
      return mass * Project::Libraries::Constants::GRAVITY;
    }

    void applyForce(float fx, float fy) {
      force.x += fx;
      force.y += fy;
    }

    void setVelocity(float vx, float vy) {
      velocity.set(vx, vy);;
    }

    void addVelocity(float vx, float vy) {
      velocity.x += vx;
      velocity.y += vy;
    }

    void setAcceleration(float ax, float ay) {
      acceleration.set(ax, ay);
    }

    void addAcceleration(float ax, float ay) {
      acceleration.x += ax;
      acceleration.y += ay;
    }

    void addAngularVelocity(float av) {
      angularVelocity += av;
    }

    void addAngularAcceleration(float aa) {
      angularAcceleration += aa;
    }
  };
}

#endif
