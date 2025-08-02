#ifndef PHYSICS_DATA_H
#define PHYSICS_DATA_H

#include "libraries/constants/Constants.h"

namespace Project::Components {
  struct PhysicsData {
    // Linear motion
    float forceX = 0.0f;
    float forceY = 0.0f;

    float accelerationX = 0.0f;
    float accelerationY = 0.0f;

    float velocityX = 0.0f;
    float velocityY = 0.0f;

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
      forceX += fx;
      forceY += fy;
    }

    void setVelocity(float vx, float vy) {
      velocityX = vx;
      velocityY = vy;
    }

    void addVelocity(float vx, float vy) {
      velocityX += vx;
      velocityY += vy;
    }

    void setAcceleration(float ax, float ay) {
      accelerationX = ax;
      accelerationY = ay;
    }

    void addAcceleration(float ax, float ay) {
      accelerationX += ax;
      accelerationY += ay;
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
