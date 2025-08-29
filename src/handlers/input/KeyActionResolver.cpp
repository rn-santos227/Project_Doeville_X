#include "KeyActionResolver.h"

#include <algorithm>
#include <array>
#include <string>
#include <string_view>

#include "utilities/string/StringUtils.h"
#include "libraries/constants/NumericConstants.h"
#include "libraries/constants/ScanCodeKeys.h"

namespace Project::Handlers {
  namespace CodeKeys = Project::Libraries::Constants;

  KeyAction KeyActionResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, KeyAction>, Project::Libraries::Constants::KEY_ACTION_NAME_ALIAS_COUNT> map{{
      {CodeKeys::MOVE_UP, KeyAction::MOVE_UP},
      {CodeKeys::MOVE_DOWN, KeyAction::MOVE_DOWN},
      {CodeKeys::MOVE_LEFT, KeyAction::MOVE_LEFT},
      {CodeKeys::MOVE_RIGHT, KeyAction::MOVE_RIGHT},
      {CodeKeys::ESCAPE, KeyAction::ESCAPE},
      {CodeKeys::ENTER, KeyAction::ENTER},
      {CodeKeys::CONFIRM, KeyAction::CONFIRM},
      {CodeKeys::MOVE_AXIS_X, KeyAction::MOVE_AXIS_X},
      {CodeKeys::MOVE_AXIS_Y, KeyAction::MOVE_AXIS_Y},
      {CodeKeys::ACTION_1, KeyAction::ACTION_1},
      {CodeKeys::ACTION_2, KeyAction::ACTION_2},
      {CodeKeys::ACTION_3, KeyAction::ACTION_3},
      {CodeKeys::ACTION_4, KeyAction::ACTION_4},
      {CodeKeys::LEFT_ACTION_1, KeyAction::LEFT_ACTION_1},
      {CodeKeys::LEFT_ACTION_2, KeyAction::LEFT_ACTION_2},
      {CodeKeys::LEFT_ACTION_3, KeyAction::LEFT_ACTION_3},
      {CodeKeys::RIGHT_ACTION_1, KeyAction::RIGHT_ACTION_1},
      {CodeKeys::RIGHT_ACTION_2, KeyAction::RIGHT_ACTION_2},
      {CodeKeys::RIGHT_ACTION_3, KeyAction::RIGHT_ACTION_3},
      {CodeKeys::CAMERA_UP, KeyAction::CAMERA_UP},
      {CodeKeys::CAMERA_DOWN, KeyAction::CAMERA_DOWN},
      {CodeKeys::CAMERA_LEFT, KeyAction::CAMERA_LEFT},
      {CodeKeys::CAMERA_RIGHT, KeyAction::CAMERA_RIGHT},
      {CodeKeys::CAMERA_ZOOM_IN, KeyAction::CAMERA_ZOOM_IN},
      {CodeKeys::CAMERA_ZOOM_OUT, KeyAction::CAMERA_ZOOM_OUT},
      {CodeKeys::CAMERA_ROTATE_CCW, KeyAction::CAMERA_ROTATE_CCW},
      {CodeKeys::CAMERA_ROTATE_CW, KeyAction::CAMERA_ROTATE_CW},
      {CodeKeys::ZOOM_IN, KeyAction::CAMERA_ZOOM_IN},
      {CodeKeys::ZOOM_OUT, KeyAction::CAMERA_ZOOM_OUT},
      {CodeKeys::ROTATE_CCW, KeyAction::CAMERA_ROTATE_CCW},
      {CodeKeys::ROTATE_CW, KeyAction::CAMERA_ROTATE_CW},
      {CodeKeys::INTERACT, KeyAction::INTERACT},
      {CodeKeys::INVENTORY_OPEN, KeyAction::INVENTORY_OPEN},
      {CodeKeys::USE_ITEM, KeyAction::USE_ITEM},
      {CodeKeys::ITEM_0, KeyAction::ITEM_0},
      {CodeKeys::ITEM_1, KeyAction::ITEM_1},
      {CodeKeys::ITEM_2, KeyAction::ITEM_2},
      {CodeKeys::ITEM_3, KeyAction::ITEM_3},
      {CodeKeys::ITEM_4, KeyAction::ITEM_4},
      {CodeKeys::ITEM_5, KeyAction::ITEM_5},
      {CodeKeys::ITEM_6, KeyAction::ITEM_6},
      {CodeKeys::ITEM_7, KeyAction::ITEM_7},
      {CodeKeys::ITEM_8, KeyAction::ITEM_8},
      {CodeKeys::ITEM_9, KeyAction::ITEM_9},
      {CodeKeys::ATTACK_PRIMARY, KeyAction::ATTACK_PRIMARY},
      {CodeKeys::ATTACK_SECONDARY, KeyAction::ATTACK_SECONDARY},
      {CodeKeys::BLOCK, KeyAction::BLOCK},
      {CodeKeys::DODGE, KeyAction::DODGE},
      {CodeKeys::RELOAD, KeyAction::RELOAD},
      {CodeKeys::SPRINT, KeyAction::SPRINT},
      {CodeKeys::CROUCH, KeyAction::CROUCH},
      {CodeKeys::JUMP, KeyAction::JUMP},
      {CodeKeys::SWITCH_WEAPON, KeyAction::SWITCH_WEAPON},
      {CodeKeys::MENU_OPEN, KeyAction::MENU_OPEN},
      {CodeKeys::MENU_BACK, KeyAction::MENU_BACK},
      {CodeKeys::MAP_TOGGLE, KeyAction::MAP_TOGGLE}
    }};

    std::string normalized(name);
    std::replace(normalized.begin(), normalized.end(), '-', '_');
    std::replace(normalized.begin(), normalized.end(), ' ', '_');

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, normalized)) return value;
    }
    return KeyAction::NONE;
  }
}
