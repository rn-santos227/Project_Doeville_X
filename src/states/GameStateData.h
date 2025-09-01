#ifndef GAME_STATE_DATA_H
#define GAME_STATE_DATA_H

#include "GameStateCategory.h"
#include "DimensionMode.h"

#include <string>
#include <SDL2/SDL.h>

#include "libraries/constants/ColorConstants.h"
#include "libraries/constants/TimeConstants.h"

namespace Project::States {
  struct GameStateData {
    std::string stateName;
    GameStateCategory gameStateCategory = GameStateCategory::DEBUG_STATE;
    DimensionMode dimensionMode = DimensionMode::BOXED;
    
    SDL_Rect mapRect{0, 0, 0, 0};
    SDL_Color backgroundColor = Project::Libraries::Constants::DEFAULT_BACKGROUND_COLOR;
    
    float darkness = 0.0f;
    float timeOfDay = Project::Libraries::Constants::DEFAULT_TIME_OF_DAY_MINUTES;
    float dayLapseSeconds = Project::Libraries::Constants::DEFAULT_DAY_LAPSE_SECONDS;

    bool active = false;
    bool initialized = false;
    bool timeCycleActive = true;
  };
}

#endif
