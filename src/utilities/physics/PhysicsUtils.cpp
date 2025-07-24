#include "PhysicsUtils.h"

#include <algorithm>
#include <cmath>

#include "libraries/constants/Constants.h"
#include "utilities/geometry/GeometryUtils.h"
#include "utilities/math/MathUtils.h"

namespace Project::Utilities {
  using Project::Utilities::MathUtils;
  using Project::Utilities::OrientedBox;

  namespace Constants = Project::Libraries::Constants;

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

  bool PhysicsUtils::checkCollision(const OrientedBox& a, const OrientedBox& b) {
    auto overlapOnAxis = [](const SDL_FPoint* pts1, const SDL_FPoint* pts2, SDL_FPoint axis) {
      float minA = std::numeric_limits<float>::max();
      float maxA = std::numeric_limits<float>::lowest();
      for (int i = 0; i < Constants::INDEX_FOUR; ++i) {
        float proj = MathUtils::dot(pts1[i].x, pts1[i].y, axis.x, axis.y);
        if (proj < minA) minA = proj;
        if (proj > maxA) maxA = proj;
      }
      float minB = std::numeric_limits<float>::max();
      float maxB = std::numeric_limits<float>::lowest();
      for (int i = 0; i < Constants::INDEX_FOUR; ++i) {
        float proj = MathUtils::dot(pts2[i].x, pts2[i].y, axis.x, axis.y);
        if (proj < minB) minB = proj;
        if (proj > maxB) maxB = proj;
      }
      return !(maxA < minB || maxB < minA);
    };

    auto computeAABB = [](const OrientedBox& box, float& minX, float& maxX, float& minY, float& maxY) {
      minX = std::numeric_limits<float>::max();
      minY = std::numeric_limits<float>::max();
      maxX = std::numeric_limits<float>::lowest();
      maxY = std::numeric_limits<float>::lowest();
      for (int i = 0; i < Constants::INDEX_FOUR; ++i) {
        float x = box.corners[i].x;
        float y = box.corners[i].y;
        if (x < minX) minX = x;
        if (x > maxX) maxX = x;
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
      }
    };

    float minAx, maxAx, minAy, maxAy;
    float minBx, maxBx, minBy, maxBy;
    computeAABB(a, minAx, maxAx, minAy, maxAy);
    computeAABB(b, minBx, maxBx, minBy, maxBy);

    if (maxAx < minBx || maxBx < minAx || maxAy < minBy || maxBy < minAy) {
      return false;
    }

    SDL_FPoint axes[Constants::INDEX_FOUR];
    for (int i = 0; i < 2; ++i) {
      const SDL_FPoint& p1 = a.corners[i];
      const SDL_FPoint& p2 = a.corners[(i + 1) % Constants::INDEX_FOUR];
      axes[i].x = -(p2.y - p1.y);
      axes[i].y = p2.x - p1.x;
    }
    
    for (int i = 0; i < 2; ++i) {
      const SDL_FPoint& p1 = b.corners[i];
      const SDL_FPoint& p2 = b.corners[(i + 1) % Constants::INDEX_FOUR];
      axes[i + 2].x = -(p2.y - p1.y);
      axes[i + 2].y = p2.x - p1.x;
    }

    for (SDL_FPoint& axis : axes) {
      float len = MathUtils::magnitude(axis.x, axis.y);
      if (len == 0.0f) continue;
      SDL_FPoint norm = MathUtils::normalize(axis.x, axis.y);
      axis = norm;
      if (!overlapOnAxis(a.corners, b.corners, axis)) return false;
    }

    return true;
  }


  float PhysicsUtils::calculateDistance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
  }

  SDL_FPoint PhysicsUtils::applyGravity(SDL_FPoint velocity, float deltaTime) {
    velocity.x += Constants::DEFAULT_GRAVITY_DIRECTION.x * Constants::GRAVITY * deltaTime;
    velocity.y += Constants::DEFAULT_GRAVITY_DIRECTION.y * Constants::GRAVITY * deltaTime;
    if (velocity.y > Constants::TERMINAL_VELOCITY) {
      velocity.y = Constants::TERMINAL_VELOCITY;
    }
    return velocity;
  }

  void PhysicsUtils::clampVelocity(SDL_FPoint& velocity, float maxSpeed) {
    float speed = MathUtils::magnitude(velocity.x, velocity.y);
    if (speed > maxSpeed) {
      float scale = maxSpeed / speed;
      velocity.x *= scale;
      velocity.y *= scale;
    }
  }

  void PhysicsUtils::clampVelocityInPlace(float& vx, float& vy, float maxVelocity) {
    float length = std::sqrt(vx * vx + vy * vy);
    if (length > maxVelocity && length > 0.0f) {
        float scale = maxVelocity / length;
        vx *= scale;
        vy *= scale;
    }
  }

  void PhysicsUtils::applyForces(
    float& velocityX, float& velocityY,
    float& accelerationX, float& accelerationY,
    float& forceX, float& forceY,
    float mass, float deltaTime) {
    accelerationX += forceX / mass;
    accelerationY += forceY / mass;
    forceX = forceY = 0.0f;

    velocityX += accelerationX * deltaTime;
    velocityY += accelerationY * deltaTime;
    accelerationX = 0.0f;
    accelerationY = 0.0f;
  }

  void PhysicsUtils::applyResistance(
    float& velocityX, float& velocityY,
    float friction, float density,
    bool isKinematic, float deltaTime) {
    if (!isKinematic && friction > 0.0f) {
      float decel = friction * deltaTime;
      if (velocityX > 0.0f) {
        velocityX -= decel;
        if (velocityX < 0.0f) velocityX = 0.0f;
      } else if (velocityX < 0.0f) {
        velocityX += decel;
        if (velocityX > 0.0f) velocityX = 0.0f;
      }
      if (velocityY > 0.0f) {
        velocityY -= decel;
        if (velocityY < 0.0f) velocityY = 0.0f;
      } else if (velocityY < 0.0f) {
        velocityY += decel;
        if (velocityY > 0.0f) velocityY = 0.0f;
      }
    }

    if (!isKinematic && density > 0.0f) {
      float factor = Constants::DEFAULT_WHOLE - density * deltaTime;
      if (factor < 0.0f) factor = 0.0f;
      velocityX *= factor;
      velocityY *= factor;
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
        result.x = (moving.x < other.x) ? -static_cast<float>(inter.w) : static_cast<float>(inter.w);
      }
    } else {
      if (dy > 0) {
        result.y = -static_cast<float>(inter.h);
      } else if (dy < 0) {
        result.y = static_cast<float>(inter.h);
      } else {
        result.y = (moving.y < other.y) ? -static_cast<float>(inter.h) : static_cast<float>(inter.h);
      }
    }
    return result;
  }

  SDL_FPoint PhysicsUtils::getCircleSnapOffset(const Circle& moving, const Circle& other, float dx, float dy) {
    SDL_FPoint result{0.0f, 0.0f};
    float cx = static_cast<float>(moving.x - other.x);
    float cy = static_cast<float>(moving.y - other.y);
    float distance = MathUtils::magnitude(cx, cy);
    float minDist = static_cast<float>(moving.r + other.r);

    float overlap = minDist - distance;
    if (overlap > 0.0f) {
      if (distance == 0.0f) {
        if (std::abs(dx) > std::abs(dy)) {
          result.x = (dx > 0 ? -overlap : overlap);
        } else {
          result.y = (dy > 0 ? -overlap : overlap);
        }
      } else {
        result.x = (dx / distance) * overlap;
        result.y = (dy / distance) * overlap;
      }
    }
    return result;
  }

  SDL_FPoint PhysicsUtils::getCircleRectSnapOffset(const Circle& moving, const SDL_Rect& other, float dx, float dy) {
    SDL_FPoint result{0.0f, 0.0f};
    float closestX = std::clamp(static_cast<float>(moving.x), static_cast<float>(other.x), static_cast<float>(other.x + other.w));
    float closestY = std::clamp(static_cast<float>(moving.y), static_cast<float>(other.y), static_cast<float>(other.y + other.h));
    float cx = static_cast<float>(moving.x) - closestX;
    float cy = static_cast<float>(moving.y) - closestY;
    float distance = MathUtils::magnitude(cx, cy);
    float overlap = static_cast<float>(moving.r) - distance;
    
    if (overlap > 0.0f) {
      if (distance == 0.0f) {
        float leftDist = static_cast<float>(moving.x - other.x);
        float rightDist = static_cast<float>(other.x + other.w - moving.x);
        float topDist = static_cast<float>(moving.y - other.y);
        float bottomDist = static_cast<float>(other.y + other.h - moving.y);
        float minDist = std::min({leftDist, rightDist, topDist, bottomDist});

        if (leftDist == minDist) {
          result.x = -(static_cast<float>(moving.r) - leftDist);
        }
         
        else if (rightDist == minDist) {
          result.x = static_cast<float>(moving.r) - rightDist;
        }

        else if (topDist == minDist) {
          result.y = -(static_cast<float>(moving.r) - topDist);
        }
          
        else {
          result.y = static_cast<float>(moving.r) - bottomDist;
        }
      } else {
        result.x = (cx / distance) * overlap;
        result.y = (cy / distance) * overlap;
      }
    }
    return result;
  }

  SDL_FPoint PhysicsUtils::getRectCircleSnapOffset(const SDL_Rect& moving, const Circle& other, float dx, float dy) {
    SDL_FPoint offset = getCircleRectSnapOffset(other, moving, -dx, -dy);
    offset.x = -offset.x;
    offset.y = -offset.y;
    return offset;
  }
}
