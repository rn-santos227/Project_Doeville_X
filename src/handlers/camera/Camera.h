#ifndef CAMERA_H
#define CAMERA_H

#include <SDL.h>
#include <array>

#include "libraries/constants/FloatConstants.h"
#include "libraries/constants/IndexConstants.h"

namespace Project::Handlers {
  struct Plane {
    float a;
    float b;
    float c;
    bool test(float x, float y) const { return a * x + b * y + c >= 0.0f; }
  };

  struct Camera {
    std::array<Plane, Project::Libraries::Constants::INDEX_FOUR> planes;
  };

  inline Camera makeCameraFromRect(const SDL_FRect& rect) {
    Camera cam;
    cam.planes[Project::Libraries::Constants::INDEX_ZERO] = Plane{Project::Libraries::Constants::DEFAULT_WHOLE, 0.0f, -rect.x};
    cam.planes[Project::Libraries::Constants::INDEX_ONE] = Plane{-Project::Libraries::Constants::DEFAULT_WHOLE, 0.0f, rect.x + rect.w};
    cam.planes[Project::Libraries::Constants::INDEX_TWO] = Plane{0.0f, Project::Libraries::Constants::DEFAULT_WHOLE, -rect.y};
    cam.planes[Project::Libraries::Constants::INDEX_THREE] = Plane{0.0f, -Project::Libraries::Constants::DEFAULT_WHOLE, rect.y + rect.h};
    return cam;
  }
}

#endif
