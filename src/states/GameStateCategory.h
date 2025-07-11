#ifndef STATE_CATEGORY_H
#define STATE_CATEGORY_H

#include <string>
#include <unordered_map>

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

  inline GameStateCategory parseGameStateCategory(const std::string& value) {
    static const std::unordered_map<std::string, GameStateCategory> map = {
      {"MAIN_MENU_STATE", GameStateCategory::MAIN_MENU_STATE},
      {"SETTINGS_STATE", GameStateCategory::SETTINGS_STATE},
      {"PAUSE_STATE", GameStateCategory::PAUSE_STATE},
      {"LOADING_STATE", GameStateCategory::LOADING_STATE},
      {"TRANSITION_STATE", GameStateCategory::TRANSITION_STATE},
      {"STATIC_GAME_STATE", GameStateCategory::STATIC_GAME_STATE},
      {"DYNAMIC_GAME_STATE", GameStateCategory::DYNAMIC_GAME_STATE},
      {"CINEMATIC_STATE", GameStateCategory::CINEMATIC_STATE},
      {"FPS_GAMEPLAY_STATE", GameStateCategory::FPS_GAMEPLAY_STATE},
      {"SIDE_SCROLLER_STATE", GameStateCategory::SIDE_SCROLLER_STATE},
      {"PUZZLE_GAMEPLAY_STATE", GameStateCategory::PUZZLE_GAMEPLAY_STATE},
      {"VR_GAMEPLAY_STATE", GameStateCategory::VR_GAMEPLAY_STATE},
      {"BATTLE_STATE", GameStateCategory::BATTLE_STATE},
      {"DIALOGUE_STATE", GameStateCategory::DIALOGUE_STATE},
      {"INVENTORY_STATE", GameStateCategory::INVENTORY_STATE},
      {"MAP_STATE", GameStateCategory::MAP_STATE},
      {"PUZZLE_STATE", GameStateCategory::PUZZLE_STATE},
      {"SHOP_STATE", GameStateCategory::SHOP_STATE},
      {"STORY_STATE", GameStateCategory::STORY_STATE},
      {"VICTORY_STATE", GameStateCategory::VICTORY_STATE},
      {"GAME_OVER_STATE", GameStateCategory::GAME_OVER_STATE},
      {"CREDITS_STATE", GameStateCategory::CREDITS_STATE},
      {"DEBUG_STATE", GameStateCategory::DEBUG_STATE},
      {"EDITOR_STATE", GameStateCategory::EDITOR_STATE}
    };

    auto it = map.find(value);
    if (it != map.end()) return it->second;
    return GameStateCategory::DEBUG_STATE;
  }
}

#endif

