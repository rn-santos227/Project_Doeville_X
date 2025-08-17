#ifndef SWEEP_AND_PRUNE_H
#define SWEEP_AND_PRUNE_H

#include <vector>
#include <utility>
#include <SDL.h>

#include "SpatialHashGrid.h"

namespace Project::Utilities {
  class SweepAndPrune {
  public:
    using Pair = std::pair<Collider, Collider>;
    static std::vector<Pair> findPairs(const std::vector<std::pair<SDL_FRect, Collider>>& objects);
  };
}

#endif
