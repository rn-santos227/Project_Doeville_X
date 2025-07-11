#ifndef SCRIPT_CATEGORY_H
#define SCRIPT_CATEGORY_H

#include <string>
#include <unordered_map>

namespace Project::Services {
  enum class ScriptCategory {
    ENTITY,
    ITEM,
    ANIMATION,
    MAP,
    STATE,
    OTHER,
    INVALID
  };
}

#endif
