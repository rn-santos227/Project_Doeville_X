#include "QuadTree.h"

#include <algorithm>
#include <unordered_set>

namespace Project::Utilities {
  QuadTree::QuadTree(const SDL_Rect& bounds, int maxDepth, int maxObjects)
      : boundary(bounds), depth(0), maxDepth(maxDepth), maxObjects(maxObjects) {}

  void QuadTree::clear() {
    objects.clear();
    children.clear();
  }

  void QuadTree::subdivide() {
    int hw = boundary.w / 2;
    int hh = boundary.h / 2;
    children.emplace_back(SDL_Rect{boundary.x, boundary.y, hw, hh}, maxDepth - 1, maxObjects);
    children.emplace_back(SDL_Rect{boundary.x + hw, boundary.y, hw, hh}, maxDepth - 1, maxObjects);
    children.emplace_back(SDL_Rect{boundary.x, boundary.y + hh, hw, hh}, maxDepth - 1, maxObjects);
    children.emplace_back(SDL_Rect{boundary.x + hw, boundary.y + hh, hw, hh}, maxDepth - 1, maxObjects);
    for (auto& child : children) child.depth = depth + 1;
  }

  void QuadTree::insert(const Collider& obj, const SDL_Rect& bounds) {
    if (!SDL_HasIntersection(&boundary, &bounds)) return;

    if (objects.size() < static_cast<size_t>(maxObjects) || depth >= maxDepth) {
      objects.push_back(obj);
      return;
    }

    if (!subdivided()) {
      subdivide();
    }

    for (auto& child : children) {
      child.insert(obj, bounds);
    }
  }

  std::vector<Collider> QuadTree::query(const SDL_Rect& area) const {
    std::vector<Collider> result;
    std::unordered_set<void*> seen;
    queryInternal(area, result, seen);
    return result;
  }

  void QuadTree::queryInternal(const SDL_Rect& area, std::vector<Collider>& result, std::unordered_set<void*>& seen) const {
    if (!SDL_HasIntersection(&boundary, &area)) return;

    for (const auto& obj : objects) {
      void* key = obj.physics ? static_cast<void*>(obj.physics) : static_cast<void*>(obj.box);
      if (seen.insert(key).second) {
        result.push_back(obj);
      }
    }

    if (subdivided()) {
      for (const auto& child : children) {
        child.queryInternal(area, result, seen);
      }
    }
  }
}
