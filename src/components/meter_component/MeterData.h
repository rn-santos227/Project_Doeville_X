#ifndef METER_DATA_H
#define METER_DATA_H

#include <SDL.h>

#include "libraries/constants/Constants.h"

namespace Project::Components {
  struct MeterData {
    SDL_Rect rect{0, 0, 0, 0};
    SDL_Color barColor = Project::Libraries::Constants::COLOR_GREEN;
    SDL_Color backgroundColor = Project::Libraries::Constants::COLOR_GRAY;
    SDL_Color borderColor = Project::Libraries::Constants::COLOR_WHITE;
    
    float interpolationSpeed = Project::Libraries::Constants::DEFAULT_BAR_SPEED;

    int borderWidth = 0;
    int currentBarWidth = 0;
  };
}

#endif
