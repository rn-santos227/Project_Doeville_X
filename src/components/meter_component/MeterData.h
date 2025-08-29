#ifndef METER_DATA_H
#define METER_DATA_H

#include <SDL.h>

#include "libraries/constants/Constants.h"

namespace Project::Components {
  enum class MeterOrientation {
    HORIZONTAL,
    VERTICAL
  };

  struct MeterData {
    SDL_Rect rect{0, 0, 0, 0};
    SDL_Color barColor = Project::Libraries::Constants::COLOR_GREEN;
    SDL_Color backgroundColor = Project::Libraries::Constants::COLOR_GRAY;
    SDL_Color borderColor = Project::Libraries::Constants::COLOR_WHITE;
    
    MeterOrientation orientation = MeterOrientation::VERTICAL;

    float interpolationSpeed = Project::Libraries::Constants::DEFAULT_BAR_SPEED;

    int offsetX{0};
    int offsetY{0};

    int borderWidth = 0;
    int cornerRadius = 0;
    int currentValue = 0;
    int currentAngle = 0;
    int currentBarWidth = 0;

    bool isRound = false;
  };
}

#endif
