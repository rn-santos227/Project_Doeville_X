#ifndef GEOMETRY_UTILS_H
#define GEOMETRY_UTILS_H

#include <vector>

#include <SDL.h>
#include "libraries/constants/IndexConstants.h"

namespace Project::Utilities {
  struct Capsule {
    SDL_FPoint start;
    SDL_FPoint end;
    float r;
  };

  struct Circle {
    float x;
    float y;
    float r;
  };

  struct OrientedBox {
    SDL_FPoint corners[Project::Libraries::Constants::INDEX_FOUR];
  };

  struct Polygon {
    std::vector<SDL_FPoint> vertices;
  };

  class GeometryUtils {
  public:
    static Circle makeCircle(float x, float y, float r);
    
    static SDL_FRect makeRect(float x, float y, float w, float h);
    static SDL_FRect capsuleBounds(const Capsule& cap);
    static SDL_FRect polygonBounds(const Polygon& poly);

    static float distance(float x1, float y1, float x2, float y2);
    static float pointSegmentDistance(float px, float py, float x1, float y1, float x2, float y2);

    static bool circleIntersect(const Circle& a, const Circle& b);
    static bool rectIntersect(const SDL_FRect& a, const SDL_FRect& b);
    static bool rectCircleIntersect(const SDL_FRect& rect, const Circle& c);
    static bool polygonIntersect(const Polygon& a, const Polygon& b);
    static bool polygonCircleIntersect(const Polygon& poly, const Circle& c);
    static bool capsuleCircleIntersect(const Capsule& cap, const Circle& c);

    static void renderCircle(SDL_Renderer* renderer, int cx, int cy, int r);
    static void renderFilledCircle(SDL_Renderer* renderer, int cx, int cy, int r);
  
    static void renderRoundedRect(SDL_Renderer* renderer, const SDL_FRect& rect, int r);
    static void renderFilledRoundedRect(SDL_Renderer* renderer, const SDL_FRect& rect, int r);
  };
}

#endif
