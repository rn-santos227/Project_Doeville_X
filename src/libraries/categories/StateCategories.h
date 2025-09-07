#ifndef STATE_CATEGORIES_H
#define STATE_CATEGORIES_H

namespace Project::Libraries::Categories {
  namespace States {
    constexpr size_t STATE_NAME_ALIAS_COUNT = 24;
    
    constexpr const char* MAIN_MENU_STATE = "MAIN_MENU_STATE";
    constexpr const char* SETTINGS_STATE = "SETTINGS_STATE";
    constexpr const char* PAUSE_STATE = "PAUSE_STATE";
    constexpr const char* LOADING_STATE = "LOADING_STATE";
    constexpr const char* TRANSITION_STATE = "TRANSITION_STATE";
    constexpr const char* STATIC_GAME_STATE = "STATIC_GAME_STATE";
    constexpr const char* DYNAMIC_GAME_STATE = "DYNAMIC_GAME_STATE";
    constexpr const char* CINEMATIC_STATE = "CINEMATIC_STATE";
    constexpr const char* FPS_GAMEPLAY_STATE = "FPS_GAMEPLAY_STATE";
    constexpr const char* SIDE_SCROLLER_STATE = "SIDE_SCROLLER_STATE";
    constexpr const char* PUZZLE_GAMEPLAY_STATE = "PUZZLE_GAMEPLAY_STATE";
    constexpr const char* VR_GAMEPLAY_STATE = "VR_GAMEPLAY_STATE";
    constexpr const char* BATTLE_STATE = "BATTLE_STATE";
    constexpr const char* DIALOGUE_STATE = "DIALOGUE_STATE";
    constexpr const char* INVENTORY_STATE = "INVENTORY_STATE";
    constexpr const char* MAP_STATE = "MAP_STATE";
    constexpr const char* PUZZLE_STATE = "PUZZLE_STATE";
    constexpr const char* SHOP_STATE = "SHOP_STATE";
    constexpr const char* STORY_STATE = "STORY_STATE";
    constexpr const char* VICTORY_STATE = "VICTORY_STATE";
    constexpr const char* GAME_OVER_STATE = "GAME_OVER_STATE";
    constexpr const char* CREDITS_STATE = "CREDITS_STATE";
    constexpr const char* DEBUG_STATE = "DEBUG_STATE";
    constexpr const char* EDITOR_STATE = "EDITOR_STATE";
  }
}

#endif
