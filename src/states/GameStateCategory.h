#ifndef STATE_CATEGORY_H
#define STATE_CATEGORY_H

namespace Project::States {
  enum class GameStateCategory {
    // Core States
    MAIN_MENU_STATE,
    SETTINGS_STATE,
    PAUSE_STATE,
    LOADING_STATE,
    TRANSITION_STATE,

    // Gameplay States
    STATIC_GAME_STATE,
    DYNAMIC_GAME_STATE,
    CINEMATIC_STATE,
    FPS_GAMEPLAY_STATE,
    SIDE_SCROLLER_STATE,
    PUZZLE_GAMEPLAY_STATE,
    VR_GAMEPLAY_STATE,

    // Specialty States
    BATTLE_STATE,
    DIALOGUE_STATE,
    INVENTORY_STATE,
    MAP_STATE,
    PUZZLE_STATE,
    SHOP_STATE,
    STORY_STATE,

    // End Game States
    VICTORY_STATE,
    GAME_OVER_STATE,

    // Miscellaneous States
    CREDITS_STATE,
    DEBUG_STATE,
    EDITOR_STATE,
  };
}

#endif

