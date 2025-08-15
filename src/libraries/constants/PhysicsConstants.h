#ifndef PHYSICS_CONSTANTS_H
#define PHYSICS_CONSTANTS_H

#include <SDL.h>

namespace Project::Libraries::Constants {
  constexpr float GRAVITY = 9.81f;
  constexpr float TERMINAL_VELOCITY = 800.0f;
  constexpr float DEFAULT_MOTION_SPEED = 100.0f;
  constexpr float DEFAULT_ROTATION_SPEED = 90.0f;
  constexpr float DEFAULT_ACCELERATION = 10.0f;
  constexpr float DEFAULT_BOUNCE_FACTOR = 0.5f;
  constexpr float DEFAULT_BRAKE_POWER = 1.0f;
  constexpr float DEFAULT_FRICTION = 0.1f;
  constexpr float DEFAULT_DAMPING = 0.1f;
  constexpr float DEFAULT_COLLISION_THRESHOLD = 0.1f;
  constexpr float DEFAULT_PUSH_FORCE = 1.0f;
  constexpr float DEFAULT_MASS = 1.0f;
  constexpr float DEFAULT_DENSITY = 0.1f;
  constexpr float DEFAULT_GRAVITY_SCALE = 1.0f;
  constexpr float DEFAULT_ROTATION = 0.0f;
  constexpr float DEFAULT_FLEXIBILITY = 0.5f;
  constexpr float DEFAULT_SPIN = 0.0;

  constexpr float CENTER_FACTOR = Constants::DEFAULT_HALF;
  constexpr float HIGH_TICK_RATE = 0.0f;
  constexpr float LOW_TICK_FREQUENCY = 15.0f;
  constexpr float DEFAULT_TICK_FREQUENCY = 60.0f;
  constexpr float LOW_TICK_RATE = Constants::DEFAULT_WHOLE / LOW_TICK_FREQUENCY;
  constexpr float DEFAULT_TICK_RATE = Constants::DEFAULT_WHOLE / DEFAULT_TICK_FREQUENCY;
  constexpr float FAR_DISTANCE_THRESHOLD = 1000.0f;
  constexpr float MID_DISTANCE_THRESHOLD = 500.0f;
  constexpr float MID_TICK_MULTIPLIER = Constants::DEFAULT_DOUBLE;
  constexpr float FAR_TICK_MULTIPLIER = Constants::DEFAULT_DOUBLE * Constants::DEFAULT_DOUBLE;

  constexpr SDL_FPoint DEFAULT_GRAVITY_DIRECTION{0.0f, 1.0f};
}

#endif
