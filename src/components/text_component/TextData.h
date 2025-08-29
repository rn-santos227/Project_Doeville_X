#include <string>
#include <SDL.h>

#include "libraries/constants/Constants.h"

namespace Project::Components {
  struct TextData {
    SDL_Rect rect{0, 0, 0, 0};
    SDL_Rect textRect{0, 0, 0, 0};

    SDL_Color textColor = Project::Libraries::Constants::COLOR_WHITE;
    SDL_Color borderColor = Project::Libraries::Constants::COLOR_BLACK;
    SDL_Color bgColor{0, 0, 0, 0};

    std::string currentText;
    std::string fontPath;

    int fontSize = 0;
    int borderWidth = 0;
    int borderRadius = 0;

    int paddingTop{0};
    int paddingRight{0};
    int paddingBottom{0};
    int paddingLeft{0};

    int offsetX{0};
    int offsetY{0};
  };
}

