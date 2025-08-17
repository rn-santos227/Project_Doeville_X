#ifndef QUAD_TREE_H
#define QUAD_TREE_H


#include <vector>
#include <unordered_set>

#include <SDL.h>

#include "SpatialHashGrid.h"
#include "libraries/constants/IndexConstants.h"

namespace Project::Utilities {
  class QuadTree {
  public:
    QuadTree(
      const SDL_FRect& bounds, 
      int maxDepth = Project::Libraries::Constants::INDEX_FIVE,
      int maxObjects = Project::Libraries::Constants::INDEX_FOUR
    );

    void clear();
    void insert(const Collider& obj, const SDL_FRect& bounds);
    std::vector<Collider> query(const SDL_FRect& area) const;

  private:
    SDL_FRect boundary;
    
    int depth;
    int maxDepth;
    int maxObjects;
    std::vector<Collider> objects;
    std::vector<QuadTree> children;

    bool subdivided() const { return !children.empty(); }
    void subdivide();
    void queryInternal(const SDL_FRect& area, std::vector<Collider>& result, std::unordered_set<void*>& seen) const;
  };
}

#endif
