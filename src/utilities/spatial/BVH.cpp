#include "BVH.h"

#include "libraries/constants/IndexConstants.h"

#include <algorithm>

namespace Project::Utilities {
  using ObjPair = std::pair<SDL_Rect, Collider>;

  static SDL_Rect unionRect(const SDL_Rect& a, const SDL_Rect& b) {
    const int left = std::min(a.x, b.x);
    const int top = std::min(a.y, b.y);
    const int right = std::max(a.x + a.w, b.x + b.w);
    const int bottom = std::max(a.y + a.h, b.y + b.h);
    return {left, top, right - left, bottom - top};
  }

  void BVH::clear() {
    root.reset();
  }

  std::unique_ptr<BVHNode> BVH::buildRecursive(std::vector<ObjPair>& objs, int start, int end) {
    if (start >= end) return nullptr;
    if (end - start == 1) {
      auto node = std::make_unique<BVHNode>();
      node->bounds = objs[start].first;
      node->collider = objs[start].second;
      return node;
    }

    SDL_Rect bounds = objs[start].first;
    for (int i = start + 1; i < end; ++i) {
      bounds = unionRect(bounds, objs[i].first);
    }

    int axis = (bounds.w > bounds.h) ? 0 : 1;
    int mid = (start + end) / Project::Libraries::Constants::INDEX_TWO;
    std::nth_element(objs.begin() + start, objs.begin() + mid, objs.begin() + end,
      [axis](const ObjPair& a, const ObjPair& b) {
        return axis == 0 ? a.first.x < b.first.x : a.first.y < b.first.y;
      });

    auto node = std::make_unique<BVHNode>();
    node->bounds = bounds;
    node->left = buildRecursive(objs, start, mid);
    node->right = buildRecursive(objs, mid, end);
    return node;
  }

  void BVH::build(const std::vector<ObjPair>& objects) {
    std::vector<ObjPair> objs = objects;
    root = buildRecursive(objs, 0, static_cast<int>(objs.size()));
  }

  void BVH::queryRecursive(const BVHNode* node, const SDL_Rect& area, std::vector<Collider>& out) const {
    if (!node) return;
    if (!SDL_HasIntersection(&node->bounds, &area)) return;
    if (node->isLeaf()) {
      out.push_back(node->collider);
      return;
    }
    queryRecursive(node->left.get(), area, out);
    queryRecursive(node->right.get(), area, out);
  }

  std::vector<Collider> BVH::query(const SDL_Rect& area) const {
    std::vector<Collider> result;
    queryRecursive(root.get(), area, result);
    return result;
  }
}
