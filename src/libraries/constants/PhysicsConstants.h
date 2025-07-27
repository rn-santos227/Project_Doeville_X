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

  constexpr float DEFAULT_DENOMINATOR = 2.0f;
  constexpr float DEFAULT_WHOLE = 1.0f;
  constexpr float DEFAULT_HALF = 0.5f;
  constexpr SDL_FPoint DEFAULT_GRAVITY_DIRECTION{0.0f, 1.0f};
}

#endif
