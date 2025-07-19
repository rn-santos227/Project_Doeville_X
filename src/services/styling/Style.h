#ifndef STYLE_H
#define STYLE_H

#include <SDL.h>

namespace Project::Services {
  struct Style {
    SDL_Color background{0, 0, 0, 0};
    SDL_Color foreground{0, 0, 0, 0};
    SDL_Color gradientStart{0, 0, 0, 0};
    SDL_Color gradientEnd{0, 0, 0, 0};

    int margin{0};
    int padding{0};
    
    float radius{0.0f};
  };
}

#endif
