#ifndef QUAD_TREE_H
#define QUAD_TREE_H


#include <vector>
#include <unordered_set>

#include <SDL.h>

#include "SpatialHashGrid.h"

namespace Project::Utilities {
  class QuadTree {
  public:
    QuadTree(const SDL_Rect& bounds, int maxDepth = 5, int maxObjects = 4);

    void clear();
    void insert(const Collider& obj, const SDL_Rect& bounds);
    std::vector<Collider> query(const SDL_Rect& area) const;

  private:
    SDL_Rect boundary;
    int depth;
    int maxDepth;
    int maxObjects;
    std::vector<Collider> objects;
    std::vector<QuadTree> children;

    bool subdivided() const { return !children.empty(); }
    void subdivide();
    void queryInternal(const SDL_Rect& area, std::vector<Collider>& result, std::unordered_set<void*>& seen) const;
  };
}

#endif
