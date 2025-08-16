#ifndef MOTION_DATA_H
#define MOTION_DATA_H

#include "MovementMode.h"
#include "utilities/physics/PhysicsUtils.h"

namespace Project::Components {
  struct MotionData {
  MovementMode movementMode = MovementMode::STANDARD;

  float maxSpeed = 0.0f;
  float acceleration = 0.0f;
  float brakePower = 0.0f;
  float friction = 0.0f;

  Project::Utilities::Velocity velocity;

  float posX = 0.0f;
  float posY = 0.0f;

  bool accelerationEnabled = false;
  bool rotationEnabled = false;

  MotionData(
    float maxSpeed = 0.0f,
    float acceleration = 0.0f,
    float brakePower = 0.0f,
    float friction = 0.0f,
    float px = 0.0f,
    float py = 0.0f,
    bool accelEnabled = false,
    bool rotateEnabled = false,
    MovementMode mode = MovementMode::STANDARD
  )
    : maxSpeed(maxSpeed),
    acceleration(acceleration),
    brakePower(brakePower),
    friction(friction),
    posX(px),
    posY(py),
    accelerationEnabled(accelEnabled),
    rotationEnabled(rotateEnabled),
    movementMode(mode)
    {}
  };
}

#endif

