#ifndef SPRITE_ASSET_DATA_H
#define SPRITE_ASSET_DATA_H

#include <SDL.h>

namespace Project::Assets {
  struct TileAssetData {
    SDL_Rect rect;
    
    int x = 0;
    int y = 0;

    int width = 0;
    int height = 0;
    
    bool passable = true;
  };
}

#endif
