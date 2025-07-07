#include "PhysicsUtils.h"

#include <cmath>

namespace Project::Utilities {
  bool PhysicsUtils::checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    SDL_Rect intersection;
    return SDL_IntersectRect(&a, &b, &intersection);
  }

  float PhysicsUtils::calculateDistance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
  }

  SDL_FPoint PhysicsUtils::applyGravity(SDL_FPoint velocity, float deltaTime) {
    velocity.y += Project::Libraries::Constants::GRAVITY * deltaTime;
    if (velocity.y > Project::Libraries::Constants::TERMINAL_VELOCITY) {
      velocity.y = Project::Libraries::Constants::TERMINAL_VELOCITY;
    }
    return velocity;
  }

  void PhysicsUtils::clampVelocity(SDL_FPoint& velocity, float maxSpeed) {
    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (speed > maxSpeed) {
      float scale = maxSpeed / speed;
      velocity.x *= scale;
      velocity.y *= scale;
    }
  }
}
