#include "GameStateCategoryResolver.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>

#include "libraries/categories/Categories.h"

namespace Project::States {
  namespace States = Project::Libraries::Categories::States;
  GameStateCategory GameStateCategoryResolver::resolve(const std::string& name) {
    static const std::unordered_map<std::string, GameStateCategory> map = {
      {std::string(States::MAIN_MENU_STATE), GameStateCategory::MAIN_MENU_STATE},
      {std::string(States::SETTINGS_STATE), GameStateCategory::SETTINGS_STATE},
      {std::string(States::PAUSE_STATE), GameStateCategory::PAUSE_STATE},
      {std::string(States::LOADING_STATE), GameStateCategory::LOADING_STATE},
      {std::string(States::TRANSITION_STATE), GameStateCategory::TRANSITION_STATE},
      {std::string(States::STATIC_GAME_STATE), GameStateCategory::STATIC_GAME_STATE},
      {std::string(States::DYNAMIC_GAME_STATE), GameStateCategory::DYNAMIC_GAME_STATE},
      {std::string(States::CINEMATIC_STATE), GameStateCategory::CINEMATIC_STATE},
      {std::string(States::FPS_GAMEPLAY_STATE), GameStateCategory::FPS_GAMEPLAY_STATE},
      {std::string(States::SIDE_SCROLLER_STATE), GameStateCategory::SIDE_SCROLLER_STATE},
      {std::string(States::PUZZLE_GAMEPLAY_STATE), GameStateCategory::PUZZLE_GAMEPLAY_STATE},
      {std::string(States::VR_GAMEPLAY_STATE), GameStateCategory::VR_GAMEPLAY_STATE},
      {std::string(States::BATTLE_STATE), GameStateCategory::BATTLE_STATE},
      {std::string(States::DIALOGUE_STATE), GameStateCategory::DIALOGUE_STATE},
      {std::string(States::INVENTORY_STATE), GameStateCategory::INVENTORY_STATE},
      {std::string(States::MAP_STATE), GameStateCategory::MAP_STATE},
      {std::string(States::PUZZLE_STATE), GameStateCategory::PUZZLE_STATE},
      {std::string(States::SHOP_STATE), GameStateCategory::SHOP_STATE},
      {std::string(States::STORY_STATE), GameStateCategory::STORY_STATE},
      {std::string(States::VICTORY_STATE), GameStateCategory::VICTORY_STATE},
      {std::string(States::GAME_OVER_STATE), GameStateCategory::GAME_OVER_STATE},
      {std::string(States::CREDITS_STATE), GameStateCategory::CREDITS_STATE},
      {std::string(States::DEBUG_STATE), GameStateCategory::DEBUG_STATE},
      {std::string(States::EDITOR_STATE), GameStateCategory::EDITOR_STATE}
    };

    std::string key = name;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
      return std::toupper(c);
    });

    auto it = map.find(key);
    if (it != map.end()) return it->second;
    return GameStateCategory::DEBUG_STATE;
  }
}
