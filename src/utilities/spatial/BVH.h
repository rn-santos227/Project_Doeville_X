#ifndef BVH_H
#define BVH_H

#include <memory>
#include <vector>

#include <SDL.h>

#include "SpatialHashGrid.h"

namespace Project::Utilities {
  struct BVHNode {
    SDL_Rect bounds{0,0,0,0};
    Collider collider{};
    std::unique_ptr<BVHNode> left;
    std::unique_ptr<BVHNode> right;
    bool isLeaf() const { return !left && !right && (collider.box || collider.physics); }
  };

  class BVH {

  };
}

#endif
