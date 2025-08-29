#ifndef VISION_DATA_H
#define VISION_DATA_H

#include <SDL.h>

#include "libraries/constants/Constants.h"

namespace Project::Components {
  enum class VisionShape {
    CIRCLE,
    CONE
  };

  struct VisionData {
    VisionShape shape = VisionShape::CONE;
    SDL_FPoint position{0.0f, 0.0f};
    
    float radius = Project::Libraries::Constants::DEFAULT_LIGHT_RADIUS;
    float angle = Project::Libraries::Constants::ANGLE_90_DEG;
    float direction = Project::Libraries::Constants::ANGLE_0_DEG;
    int rays = Project::Libraries::Constants::DEFAULT_LIGHT_RAY_COUNT;
    bool revealDarkness = false;
  };
}

#endif
