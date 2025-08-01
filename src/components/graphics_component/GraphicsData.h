#ifndef GRAPHICS_DATA_H
#define GRAPHICS_DATA_H

#include <SDL.h>
#include <string>
#include <vector>

#include "handlers/animation/AnimationHandler.h"
#include "services/styling/Style.h"
#include "libraries/constants/Constants.h"

namespace Project::Components {
  struct GraphicsData {
    SDL_Color shapeColor = Project::Libraries::Constants::DEFAULT_SHAPE_COLOR;
    SDL_Color borderColor = Project::Libraries::Constants::COLOR_BLACK;
    SDL_Color gradientStart{0, 0, 0, 0};
    SDL_Color gradientEnd{0, 0, 0, 0};

    SDL_Rect destRect{0, 0, 0, 0};
    std::string pendingTexturePath;

    Project::Services::Style style{};
    Project::Services::GradientType gradient = Project::Services::GradientType::NONE;

    std::vector<int> shapeIndices;

    float radius = 0.0f;
    float rotation = 0.0f;

    int borderWidth = 0;

    bool verticesDirty = true;
    bool rotationEnabled = false;
    bool isCircle = false;
    bool drawShape = false;
    bool useGradient = false;
  };
}

#endif
