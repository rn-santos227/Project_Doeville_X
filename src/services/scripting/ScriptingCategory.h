#ifndef SCRIPT_CATEGORY_H
#define SCRIPT_CATEGORY_H

namespace Project::Services {
  enum class ScriptCategory {
    STATE,
    LAYER,
    ENTITY,
    ITEM,
    ASSET,
    OTHER,
    INVALID
  };
}

#endif
