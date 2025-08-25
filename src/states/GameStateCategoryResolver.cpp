#include "GameStateCategoryResolver.h"

#include <array>
#include <string_view>

#include "libraries/categories/Categories.h"
#include "libraries/constants/NumericConstants.h"
#include "utilities/string/StringUtils.h"

namespace Project::States {
  namespace States = Project::Libraries::Categories::States;
  namespace Constants = Project::Libraries::Constants;

  GameStateCategory GameStateCategoryResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, GameStateCategory>, Constants::STATE_NAME_ALIAS_COUNT> map{{
      {States::MAIN_MENU_STATE, GameStateCategory::MAIN_MENU_STATE},
      {States::SETTINGS_STATE, GameStateCategory::SETTINGS_STATE},
      {States::PAUSE_STATE, GameStateCategory::PAUSE_STATE},
      {States::LOADING_STATE, GameStateCategory::LOADING_STATE},
      {States::TRANSITION_STATE, GameStateCategory::TRANSITION_STATE},
      {States::STATIC_GAME_STATE, GameStateCategory::STATIC_GAME_STATE},
      {States::DYNAMIC_GAME_STATE, GameStateCategory::DYNAMIC_GAME_STATE},
      {States::CINEMATIC_STATE, GameStateCategory::CINEMATIC_STATE},
      {States::FPS_GAMEPLAY_STATE, GameStateCategory::FPS_GAMEPLAY_STATE},
      {States::SIDE_SCROLLER_STATE, GameStateCategory::SIDE_SCROLLER_STATE},
      {States::PUZZLE_GAMEPLAY_STATE, GameStateCategory::PUZZLE_GAMEPLAY_STATE},
      {States::VR_GAMEPLAY_STATE, GameStateCategory::VR_GAMEPLAY_STATE},
      {States::BATTLE_STATE, GameStateCategory::BATTLE_STATE},
      {States::DIALOGUE_STATE, GameStateCategory::DIALOGUE_STATE},
      {States::INVENTORY_STATE, GameStateCategory::INVENTORY_STATE},
      {States::MAP_STATE, GameStateCategory::MAP_STATE},
      {States::PUZZLE_STATE, GameStateCategory::PUZZLE_STATE},
      {States::SHOP_STATE, GameStateCategory::SHOP_STATE},
      {States::STORY_STATE, GameStateCategory::STORY_STATE},
      {States::VICTORY_STATE, GameStateCategory::VICTORY_STATE},
      {States::GAME_OVER_STATE, GameStateCategory::GAME_OVER_STATE},
      {States::CREDITS_STATE, GameStateCategory::CREDITS_STATE},
      {States::DEBUG_STATE, GameStateCategory::DEBUG_STATE},
      {States::EDITOR_STATE, GameStateCategory::EDITOR_STATE}
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return GameStateCategory::DEBUG_STATE;
  }
}
