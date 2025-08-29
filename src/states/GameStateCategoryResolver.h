#ifndef GAME_STATE_CATEGORY_RESOLVER_H
#define GAME_STATE_CATEGORY_RESOLVER_H

#include "GameStateCategory.h"
#include <string_view>

namespace Project::States {
  class GameStateCategoryResolver {
  public:
    static GameStateCategory resolve(std::string_view name);
  };
}

#endif
