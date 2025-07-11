#ifndef GEOMETRY_UTILS_H
#define GEOMETRY_UTILS_H

#include <SDL.h>

namespace Project::Utilities {
  struct Circle {
    int x;
    int y;
    int r;
  };

  class GeometryUtils {
  public:
    static Circle makeCircle(int x, int y, int r);
    static SDL_Rect makeRect(int x, int y, int w, int h);

    static bool circleIntersect(const Circle& a, const Circle& b);
    static bool rectIntersect(const SDL_Rect& a, const SDL_Rect& b);
    static bool rectCircleIntersect(const SDL_Rect& rect, const Circle& c);

    static float distance(float x1, float y1, float x2, float y2);
  };
}

#endif
