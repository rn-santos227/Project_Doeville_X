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
    };
  }
}
