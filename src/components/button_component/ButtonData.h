#ifndef BUTTON_DATA_H
#define BUTTON_DATA_H

#include <string>
#include <SDL.h>
#include "libraries/constants/Constants.h"

namespace Project::Components {
  struct ButtonData {
    SDL_Rect rect{0, 0, 0, 0};
    SDL_Rect textRect{0, 0, 0, 0};

    SDL_Color color = Project::Libraries::Constants::COLOR_WHITE;
    SDL_Color hoverColor = Project::Libraries::Constants::COLOR_WHITE;
    SDL_Color borderColor = Project::Libraries::Constants::COLOR_WHITE;
    SDL_Color fontColor = Project::Libraries::Constants::COLOR_WHITE;
    SDL_Color fontHoverColor = Project::Libraries::Constants::COLOR_WHITE;

    std::string luaFunction;
    std::string text;

    int offsetX{0};
    int offsetY{0};

    int fontSize = Project::Libraries::Constants::DEBUG_FONT_SIZE;
    int borderWidth = 0;
    int borderRadius = 0;

    int paddingTop = 0;
    int paddingRight = 0;
    int paddingBottom = 0;
    int paddingLeft = 0;

    int marginTop = 0;
    int marginRight = 0;
    int marginBottom = 0;
    int marginLeft = 0;

    bool hovered = false;
    bool wasPressed = false;

    ButtonData() = default;
  };
}

#endif
