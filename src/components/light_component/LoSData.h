#ifndef LOS_DATA_H
#define LOS_DATA_H

#include <SDL.h>

#include "libraries/constants/Constants.h"

namespace Project::Components {
  enum class LoSShape {
    CIRCLE,
    CONE
  };

  struct LoSData {
    LoSShape shape = LoSShape::CONE;
    float radius = Project::Libraries::Constants::DEFAULT_LIGHT_RADIUS;
    float angle = Project::Libraries::Constants::ANGLE_90_DEG;
    float direction = Project::Libraries::Constants::ANGLE_0_DEG;
    int rays = Project::Libraries::Constants::DEFAULT_LIGHT_RAY_COUNT;
    bool revealDarkness = false;
    SDL_FPoint position{0.0f, 0.0f};
  };
}

#endif
