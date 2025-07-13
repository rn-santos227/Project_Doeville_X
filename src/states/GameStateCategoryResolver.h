#ifndef GAME_STATE_CATEGORY_RESOLVER_H
#define GAME_STATE_CATEGORY_RESOLVER_H

#include "GameStateCategory.h"

namespace Project::States {
  class GameStateCategoryResolver {
  public:
    static GameStateCategory resolve(const std::string& name);
  };
}

#endif
