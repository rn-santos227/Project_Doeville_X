#ifndef TILE_HANDLER_H
#define TILE_HANDLER_H

#include <string>
#include <vector>
#include <SDL.h>

#include "assets/AssetsManager.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Handlers {
  struct BuiltTile {
    SDL_Texture* texture;
    SDL_Rect src;
    SDL_Rect dest;
    bool passable;
  };
}

#endif
