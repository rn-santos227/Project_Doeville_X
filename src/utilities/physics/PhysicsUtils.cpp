#include "PhysicsUtils.h"

#include <algorithm>
#include <cmath>

#include "libraries/constants/FloatConstants.h"
#include "libraries/constants/NumericConstants.h"
#include "utilities/geometry/GeometryUtils.h"
#include "utilities/math/MathUtils.h"

namespace Project::Utilities {
  using Project::Utilities::MathUtils;
  using Project::Utilities::OrientedBox;

  namespace Constants = Project::Libraries::Constants;

  bool PhysicsUtils::checkCollision(const SDL_FRect& a, const SDL_FRect& b) {
    SDL_FRect intersection;
    return SDL_IntersectFRect(&a, &b, &intersection);
  }

  bool PhysicsUtils::checkCollision(const Project::Utilities::Circle& a, const Project::Utilities::Circle& b) {
    float distX = a.x - b.x;
    float distY = a.y - b.y;
    float distSq = distX * distX + distY * distY;
    float radiusSum = a.r + b.r;
    return distSq <= radiusSum * radiusSum;
  }

  bool PhysicsUtils::checkCollision(const SDL_FRect& rect, const Project::Utilities::Circle& c) {
    float closestX = std::clamp(c.x, rect.x, rect.x + rect.w);
    float closestY = std::clamp(c.y, rect.y, rect.y + rect.h);
    float dx = c.x - closestX;
    float dy = c.y - closestY;
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

  Velocity PhysicsUtils::applyGravity(Velocity velocity, float deltaTime) {
    velocity.x += Constants::DEFAULT_GRAVITY_DIRECTION.x * Constants::GRAVITY * deltaTime;
    velocity.y += Constants::DEFAULT_GRAVITY_DIRECTION.y * Constants::GRAVITY * deltaTime;
    if (velocity.y > Constants::TERMINAL_VELOCITY) {
      velocity.y = Constants::TERMINAL_VELOCITY;
    }
    return velocity;
  }

  void PhysicsUtils::clampVelocity(Velocity& velocity, float maxSpeed) {
    float speed = MathUtils::magnitude(velocity.x, velocity.y);
    if (speed > maxSpeed) {
      float scale = maxSpeed / speed;
      velocity.x *= scale;
      velocity.y *= scale;
    }
  }

  void PhysicsUtils::applyForces(Velocity& velocity, Velocity& acceleration, Velocity& force, float mass, float deltaTime) {
    acceleration.x += force.x / mass;
    acceleration.y += force.y / mass;
    force.set(0.0f, 0.0f);

    velocity.x += acceleration.x * deltaTime;
    velocity.y += acceleration.y * deltaTime;
    acceleration.set(0.0f, 0.0f);
  }

  void PhysicsUtils::applyResistance(Velocity& velocity, float friction, float density, bool isKinematic, float deltaTime) {
    if (!isKinematic && friction > 0.0f) {
      float decel = friction * deltaTime;
      if (velocity.x > 0.0f) {
        velocity.x -= decel;
        if (velocity.x < 0.0f) velocity.x = 0.0f;
      } else if (velocity.x < 0.0f) {
        velocity.x += decel;
        if (velocity.x > 0.0f) velocity.x = 0.0f;
      }

      if (velocity.y > 0.0f) {
        velocity.y -= decel;
        if (velocity.y < 0.0f) velocity.y = 0.0f;
      } else if (velocity.y < 0.0f) {
        velocity.y += decel;
        if (velocity.y > 0.0f) velocity.y = 0.0f;
      }
    }

    if (!isKinematic && density > 0.0f) {
      float factor = Constants::DEFAULT_WHOLE - density * deltaTime;
      if (factor < 0.0f) factor = 0.0f;
      velocity.x *= factor;
      velocity.y *= factor;
    }
  }

  SDL_FPoint PhysicsUtils::getSnapOffset(const SDL_FRect& moving, const SDL_FRect& other, float /*dx*/, float /*dy*/) {
    SDL_FPoint result{0.0f, 0.0f};
    SDL_FRect inter;
    if (!SDL_IntersectFRect(&moving, &other, &inter)) {
      return result;
    }

    float centerAx = moving.x + moving.w * Constants::DEFAULT_HALF;
    float centerAy = moving.y + moving.h * Constants::DEFAULT_HALF;
    float centerBx = other.x + other.w * Constants::DEFAULT_HALF;
    float centerBy = other.y + other.h * Constants::DEFAULT_HALF;

    float diffX = centerAx - centerBx;
    float diffY = centerAy - centerBy;

    float overlapX = (moving.w + other.w) * Constants::DEFAULT_HALF - std::abs(diffX);
    float overlapY = (moving.h + other.h) * Constants::DEFAULT_HALF - std::abs(diffY);

    if (overlapX < overlapY) {
      result.x = (diffX < 0.0f) ? -overlapX : overlapX;
    } else {
      result.y = (diffY < 0.0f) ? -overlapY : overlapY;
    }
    
    return result;
  }

  SDL_FPoint PhysicsUtils::getCircleSnapOffset(const Circle& moving, const Circle& other, float dx, float dy) {
    SDL_FPoint result{0.0f, 0.0f};
    float cx = moving.x - other.x;
    float cy = moving.y - other.y;
    float distance = MathUtils::magnitude(cx, cy);
    float minDist = moving.r + other.r;

    float overlap = minDist - distance;
    if (overlap > 0.0f) {
      const float eps = 0.01f;
      float separation = overlap + eps;
      if (distance == 0.0f) {
        if (std::abs(dx) > std::abs(dy)) {
          result.x = (dx > 0 ? -separation : separation);
        } else {
          result.y = (dy > 0 ? -separation : separation);
        }
      } else {
        result.x = (cx / distance) * overlap;
        result.y = (cy / distance) * overlap;
      }
    }
    return result;
  }

  SDL_FPoint PhysicsUtils::getCircleRectSnapOffset(const Circle& moving, const SDL_FRect& other, float dx, float dy) {
    SDL_FPoint result{0.0f, 0.0f};
    float closestX = std::clamp(moving.x, other.x, other.x + other.w);
    float closestY = std::clamp(moving.y, other.y, other.y + other.h);
    float cx = moving.x - closestX;
    float cy = moving.y - closestY;
    float distance = MathUtils::magnitude(cx, cy);
    float overlap = moving.r - distance;
    
    if (overlap >= 0.0f) {
      const float eps = 0.01f;
      float separation = overlap + eps;
      if (distance == 0.0f) {
        float leftDist = moving.x - other.x;
        float rightDist = (other.x + other.w) - moving.x;
        float topDist = moving.y - other.y;
        float bottomDist = (other.y + other.h) - moving.y;
        float minDist = std::min({leftDist, rightDist, topDist, bottomDist});

        if (leftDist == minDist) {
          result.x = -(moving.r - leftDist + eps);
        } else if (rightDist == minDist) {
          result.x = moving.r - rightDist + eps;
        } else if (topDist == minDist) {
          result.y = -(moving.r - topDist + eps);
        } else {
          result.y = moving.r - bottomDist + eps;
        }
      } else {
        result.x = (cx / distance) * separation;
        result.y = (cy / distance) * separation;
      }
    }
    return result;
  }

  SDL_FPoint PhysicsUtils::getRectCircleSnapOffset(const SDL_FRect& moving, const Circle& other, float dx, float dy) {
    SDL_FPoint offset = getCircleRectSnapOffset(other, moving, -dx, -dy);
    offset.x = -offset.x;
    offset.y = -offset.y;
    return offset;
  }

  SDL_FPoint PhysicsUtils::getOBBSnapOffset(const OrientedBox& a, const OrientedBox& b, float /*dx*/, float /*dy*/) {
    auto project = [](const OrientedBox& box, const SDL_FPoint& axis, float& min, float& max) {
      min = std::numeric_limits<float>::max();
      max = std::numeric_limits<float>::lowest();
      for (int i = 0; i < Constants::INDEX_FOUR; ++i) {
        float proj = MathUtils::dot(box.corners[i].x, box.corners[i].y, axis.x, axis.y);
        if (proj < min) min = proj;
        if (proj > max) max = proj;
      }
    };

    SDL_FPoint axes[Constants::INDEX_FOUR];
    for (int i = 0; i < 2; ++i) {
      const SDL_FPoint& p1 = a.corners[i];
      const SDL_FPoint& p2 = a.corners[(i + 1) % Constants::INDEX_FOUR];
      axes[i] = MathUtils::normalize(-(p2.y - p1.y), p2.x - p1.x);
    }
    for (int i = 0; i < 2; ++i) {
      const SDL_FPoint& p1 = b.corners[i];
      const SDL_FPoint& p2 = b.corners[(i + 1) % Constants::INDEX_FOUR];
      axes[i + 2] = MathUtils::normalize(-(p2.y - p1.y), p2.x - p1.x);
    }

    SDL_FPoint centerA{0.0f, 0.0f};
    SDL_FPoint centerB{0.0f, 0.0f};
    for (int i = 0; i < Constants::INDEX_FOUR; ++i) {
      centerA.x += a.corners[i].x;
      centerA.y += a.corners[i].y;
      centerB.x += b.corners[i].x;
      centerB.y += b.corners[i].y;
    }

    const float quarter = Constants::DEFAULT_HALF * Constants::DEFAULT_HALF;
    centerA.x *= quarter;
    centerA.y *= quarter;
    centerB.x *= quarter;
    centerB.y *= quarter;

    float smallestOverlap = std::numeric_limits<float>::max();
    SDL_FPoint bestAxis{0.0f, 0.0f};

    for (SDL_FPoint& axis : axes) {
      float minA, maxA, minB, maxB;
      project(a, axis, minA, maxA);
      project(b, axis, minB, maxB);
      float overlap = std::min(maxA, maxB) - std::max(minA, minB);
      if (overlap < smallestOverlap) {
        smallestOverlap = overlap;
        float dir = MathUtils::dot(centerB.x - centerA.x, centerB.y - centerA.y, axis.x, axis.y);
        if (dir < 0.0f) {
          bestAxis = {-axis.x, -axis.y};
        } else {
          bestAxis = axis;
        }
      }
    }

    return {bestAxis.x * smallestOverlap, bestAxis.y * smallestOverlap};
  }
}
