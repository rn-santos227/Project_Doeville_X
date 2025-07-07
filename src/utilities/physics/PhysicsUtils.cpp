#include "PhysicsUtils.h"

#include <cmath>

namespace Project::Utilities {
  bool PhysicsUtils::checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    SDL_Rect intersection;
    return SDL_IntersectRect(&a, &b, &intersection);
  }
}
