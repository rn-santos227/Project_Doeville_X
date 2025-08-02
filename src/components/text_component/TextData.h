#include <string>
#include <SDL.h>

#include "libraries/constants/Constants.h"

namespace Project::Components {
  struct TextData {
    SDL_Rect rect{0, 0, 0, 0};

    SDL_Color textColor = Project::Libraries::Constants::COLOR_WHITE;
    SDL_Color borderColor = Project::Libraries::Constants::COLOR_BLACK;

    std::string currentText;
    std::string fontPath;
    
    int fontSize = 0;
    int borderWidth = 0;
  };
}

