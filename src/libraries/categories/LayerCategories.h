#ifndef LAYER_CATEGORIES_H
#define LAYER_CATEGORIES_H

namespace Project::Libraries::Categories {
  namespace Layers {
    constexpr size_t LAYER_NAME_ALIAS_COUNT = 7;
    
    constexpr const char* HUD = "hud";
    constexpr const char* OVERLAY = "overlay";
    constexpr const char* FOREGROUND = "foreground";
    constexpr const char* MIDGROUND = "midground";
    constexpr const char* BACKGROUND = "background";
    constexpr const char* CINEMATIC = "cinematic";
    constexpr const char* CUSTOM = "custom";
    constexpr const char* GAME = "game";
  }
}

#endif
