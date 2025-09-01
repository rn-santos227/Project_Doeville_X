#ifndef GAME_STATE_DATA_H
#define GAME_STATE_DATA_H

#include "GameStateCategory.h"
#include "DimensionMode.h"

#include <string>
#include <SDL2/SDL.h>

#include "libraries/constants/TimeConstants.h"

namespace Project::States {
  struct GameStateData {
    std::string stateName;
    GameStateCategory gameStateCategory = GameStateCategory::DEBUG_STATE;
    DimensionMode dimensionMode = DimensionMode::BOXED;
    SDL_Color backgroundColor{0, 0, 0, 255};
    SDL_Rect mapRect{0, 0, 0, 0};
    
    float darkness = 0.0f;
    float timeOfDay = Project::Libraries::Constants::DEFAULT_TIME_OF_DAY_MINUTES;
    float dayLapseSeconds = Project::Libraries::Constants::DEFAULT_DAY_LAPSE_SECONDS;
    
    bool active = false;
    bool initialized = false;
  };
}

#endif
