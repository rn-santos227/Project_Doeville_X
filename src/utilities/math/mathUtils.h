#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <SDL.h>

namespace Project::Utilities {
  class MathUtils {
  public:
    static float dot(float x1, float y1, float x2, float y2);
    static float cross(float x1, float y1, float x2, float y2);
    static float magnitude(float x, float y);
    static SDL_FPoint normalize(float x, float y);
  };
}

#endif
