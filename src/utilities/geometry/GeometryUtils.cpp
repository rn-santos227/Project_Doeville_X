#include "GeometryUtils.h"

#include <algorithm>
#include <cmath>

namespace Project::Utilities {
  Circle GeometryUtils::makeCircle(int x, int y, int r) { 
    return {x, y, r}; 
  }

  SDL_Rect GeometryUtils::makeRect(int x, int y, int w, int h) {
    SDL_Rect rect{x, y, w, h};
    return rect;
  }

  bool GeometryUtils::circleIntersect(const Circle& a, const Circle& b) {
    float dx = static_cast<float>(a.x - b.x);
    float dy = static_cast<float>(a.y - b.y);
    float distSq = dx * dx + dy * dy;
    int radiusSum = a.r + b.r;
    return distSq <= static_cast<float>(radiusSum * radiusSum);
  }

  bool GeometryUtils::rectIntersect(const SDL_Rect& a, const SDL_Rect& b) {
    SDL_Rect inter;
    return SDL_IntersectRect(&a, &b, &inter);
  }

  bool GeometryUtils::rectCircleIntersect(const SDL_Rect& rect, const Circle& c) {
    int closestX = std::clamp(c.x, rect.x, rect.x + rect.w);
    int closestY = std::clamp(c.y, rect.y, rect.y + rect.h);
    int dx = c.x - closestX;
    int dy = c.y - closestY;
    return (dx * dx + dy * dy) <= (c.r * c.r);
  }

  float GeometryUtils::distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
  }
}
