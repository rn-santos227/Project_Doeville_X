#include "KeyActionResolver.h"

#include <algorithm>
#include <cctype>

namespace Project::Handlers {
  KeyAction KeyActionResolver::resolve(const std::string& name) {
    static const std::unordered_map<std::string, KeyAction> map = {
      {"MOVE_UP", KeyAction::MOVE_UP},
      {"MOVE_DOWN", KeyAction::MOVE_DOWN},
      {"MOVE_LEFT", KeyAction::MOVE_LEFT},
      {"MOVE_RIGHT", KeyAction::MOVE_RIGHT},
      {"ESCAPE", KeyAction::ESCAPE},
      {"ENTER", KeyAction::ENTER},
      {"CONFIRM", KeyAction::CONFIRM},
      {"MOVE_AXIS_X", KeyAction::MOVE_AXIS_X},
      {"MOVE_AXIS_Y", KeyAction::MOVE_AXIS_Y},
      {"ACTION_1", KeyAction::ACTION_1},
      {"ACTION_2", KeyAction::ACTION_2},
      {"ACTION_3", KeyAction::ACTION_3},
      {"ACTION_4", KeyAction::ACTION_4},
      {"LEFT_ACTION_1", KeyAction::LEFT_ACTION_1},
      {"LEFT_ACTION_2", KeyAction::LEFT_ACTION_2},
      {"LEFT_ACTION_3", KeyAction::LEFT_ACTION_3},
      {"RIGHT_ACTION_1", KeyAction::RIGHT_ACTION_1},
      {"RIGHT_ACTION_2", KeyAction::RIGHT_ACTION_2},
      {"RIGHT_ACTION_3", KeyAction::RIGHT_ACTION_3},
      {"CAMERA_UP", KeyAction::CAMERA_UP},
      {"CAMERA_DOWN", KeyAction::CAMERA_DOWN},
      {"CAMERA_LEFT", KeyAction::CAMERA_LEFT},
      {"CAMERA_RIGHT", KeyAction::CAMERA_RIGHT},
      {"CAMERA_ZOOM_IN", KeyAction::CAMERA_ZOOM_IN},
      {"CAMERA_ZOOM_OUT", KeyAction::CAMERA_ZOOM_OUT},
      {"INTERACT", KeyAction::INTERACT},
      {"INVENTORY_OPEN", KeyAction::INVENTORY_OPEN},
      {"USE_ITEM", KeyAction::USE_ITEM},
      {"ITEM_0", KeyAction::ITEM_0},
      {"ITEM_1", KeyAction::ITEM_1},
      {"ITEM_2", KeyAction::ITEM_2},
      {"ITEM_3", KeyAction::ITEM_3},
      {"ITEM_4", KeyAction::ITEM_4},
      {"ITEM_5", KeyAction::ITEM_5},
      {"ITEM_6", KeyAction::ITEM_6},
      {"ITEM_7", KeyAction::ITEM_7},
      {"ITEM_8", KeyAction::ITEM_8},
      {"ITEM_9", KeyAction::ITEM_9},
      {"ATTACK_PRIMARY", KeyAction::ATTACK_PRIMARY},
      {"ATTACK_SECONDARY", KeyAction::ATTACK_SECONDARY},
      {"BLOCK", KeyAction::BLOCK},
      {"DODGE", KeyAction::DODGE},
      {"RELOAD", KeyAction::RELOAD},
      {"SPRINT", KeyAction::SPRINT},
      {"CROUCH", KeyAction::CROUCH},
      {"JUMP", KeyAction::JUMP},
      {"SWITCH_WEAPON", KeyAction::SWITCH_WEAPON},
      {"MENU_OPEN", KeyAction::MENU_OPEN},
      {"MENU_BACK", KeyAction::MENU_BACK},
      {"MAP_TOGGLE", KeyAction::MAP_TOGGLE}
    };

    std::string key = name;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){ return std::toupper(c); });
  }
}
