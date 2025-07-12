#include "PhysicsUtils.h"

#include <algorithm>
#include <cmath>

namespace Project::Utilities {
  bool PhysicsUtils::checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    SDL_Rect intersection;
    return SDL_IntersectRect(&a, &b, &intersection);
  }

  bool PhysicsUtils::checkCollision(const Project::Utilities::Circle& a, const Project::Utilities::Circle& b) {
    float distX = static_cast<float>(a.x - b.x);
    float distY = static_cast<float>(a.y - b.y);
    float distSq = distX * distX + distY * distY;
    int radiusSum = a.r + b.r;
    return distSq <= static_cast<float>(radiusSum * radiusSum);
  }

  bool PhysicsUtils::checkCollision(const SDL_Rect& rect, const Project::Utilities::Circle& c) {
    int closestX = std::clamp(c.x, rect.x, rect.x + rect.w);
    int closestY = std::clamp(c.y, rect.y, rect.y + rect.h);
    int dx = c.x - closestX;
    int dy = c.y - closestY;
    return (dx * dx + dy * dy) <= (c.r * c.r);
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

  SDL_FPoint PhysicsUtils::getSnapOffset(const SDL_Rect& moving, const SDL_Rect& other, float dx, float dy) {
    SDL_Rect inter;
    SDL_FPoint result{0.0f, 0.0f};
    if (!SDL_IntersectRect(&moving, &other, &inter)) {
      return result;
    }

    if (inter.w < inter.h) {
      if (dx > 0) {
        result.x = -static_cast<float>(inter.w);
      } else if (dx < 0) {
        result.x = static_cast<float>(inter.w);
      } else {
        result.x = (moving.x < other.x) ? -static_cast<float>(inter.w)
                                        : static_cast<float>(inter.w);
      }
    } else {
      if (dy > 0) {
        result.y = -static_cast<float>(inter.h);
      } else if (dy < 0) {
        result.y = static_cast<float>(inter.h);
      } else {
        result.y = (moving.y < other.y) ? -static_cast<float>(inter.h)
                                        : static_cast<float>(inter.h);
      }
    }

    return result;
  }
}
