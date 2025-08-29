#ifndef BVH_H
#define BVH_H

#include <memory>
#include <vector>

#include <SDL.h>

#include "SpatialHashGrid.h"

namespace Project::Utilities {
  struct BVHNode {
    SDL_FRect bounds{0,0,0,0};
    Collider collider{};
    std::unique_ptr<BVHNode> left;
    std::unique_ptr<BVHNode> right;
    bool isLeaf() const { return !left && !right && (collider.box || collider.physics); }
  };

  class BVH {
  public:
    void build(std::vector<std::pair<SDL_FRect, Collider>> objects);
    std::vector<Collider> query(const SDL_FRect& area) const;
    void clear();

  private:
    std::unique_ptr<BVHNode> root;
    std::unique_ptr<BVHNode> buildRecursive(std::vector<std::pair<SDL_FRect, Collider>>& objs, int start, int end);
    void queryRecursive(const BVHNode* node, const SDL_FRect& area, std::vector<Collider>& out) const;
    
  };
}

#endif
