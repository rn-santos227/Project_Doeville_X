#ifndef SCRIPT_CATEGORY_H
#define SCRIPT_CATEGORY_H

#include <string>
#include <unordered_map>

namespace Project::Services {
  enum class ScriptCategory {
    State,
    Entity,
    Map,
    Animation,
    Other,
    Invalid
  };
}

#endif