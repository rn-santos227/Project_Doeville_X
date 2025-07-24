#ifndef GEOMETRY_UTILS_H
#define GEOMETRY_UTILS_H

#include <SDL.h>
#include "libraries/constants/IndexConstants.h"

namespace Project::Utilities {
  struct Circle {
    int x;
    int y;
    int r;
  };

  struct OrientedBox {
    SDL_FPoint corners[Project::Libraries::Constants::INDEX_FOUR];
  };

  class GeometryUtils {
  public:
    static Circle makeCircle(int x, int y, int r);
    static SDL_Rect makeRect(int x, int y, int w, int h);

    static bool circleIntersect(const Circle& a, const Circle& b);
    static bool rectIntersect(const SDL_Rect& a, const SDL_Rect& b);
    static bool rectCircleIntersect(const SDL_Rect& rect, const Circle& c);
    static float distance(float x1, float y1, float x2, float y2);

    static void renderCircle(SDL_Renderer* renderer, int cx, int cy, int r);
    static void renderFilledCircle(SDL_Renderer* renderer, int cx, int cy, int r);
  };
}

#endif
