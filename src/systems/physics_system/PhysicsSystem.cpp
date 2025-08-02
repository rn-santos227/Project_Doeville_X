#include "PhysicsSystem.h"

#include <algorithm>
#include <limits>

#include "entities/Entity.h"
#include "components/physics_component/PhysicsComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "libraries/constants/Constants.h"
#include "utilities/geometry/GeometryUtils.h"

namespace Project::Systems {
  using Project::Components::PhysicsComponent;
  using Project::Components::BoundingBoxComponent;

  namespace Constants = Project::Libraries::Constants;

  PhysicsSystem::PhysicsSystem()
      : quadtree(SDL_Rect{0, 0, Constants::INT_TEN_THOUSAND, Constants::INT_TEN_THOUSAND}) {
    components.reserve(Project::Libraries::Constants::MAX_MEMORY_SPACE);
    staticColliders.reserve(Project::Libraries::Constants::MAX_MEMORY_SPACE);
  }

  void Project::Systems::PhysicsSystem::add(PhysicsComponent* component) {
    if (component) components.push_back(component);
  }

  void Project::Systems::PhysicsSystem::remove(PhysicsComponent* component) {
    components.erase(std::remove(components.begin(), components.end(), component), components.end());
  }

  void Project::Systems::PhysicsSystem::addStaticCollider(BoundingBoxComponent* box) {
    if (box) staticColliders.push_back(box);
  }

  void Project::Systems::PhysicsSystem::removeStaticCollider(BoundingBoxComponent* box) {
    staticColliders.erase(std::remove(staticColliders.begin(), staticColliders.end(), box), staticColliders.end());
  }

  void Project::Systems::PhysicsSystem::update(float deltaTime) {
    grid.clear();

    SDL_Rect worldBounds{0, 0, 0, 0};
    bool hasWorldBounds = false;

    auto accumulateBounds = [&](BoundingBoxComponent* box) {
      if (!box || !box->isActive()) return;
      SDL_Rect b{0,0,0,0};
      if (!computeBounds(box, b)) return;
      if (!hasWorldBounds) {
        worldBounds = b;
        hasWorldBounds = true;
      } else {
        worldBounds = unionRect(worldBounds, b);
      }
    };

    for (auto* comp : components) {
      if (!comp || !comp->isActive()) continue;
      auto* owner = comp->getOwner();
      if (!owner) continue;
      auto* box = owner->getBoundingBoxComponent();
      accumulateBounds(box);
    }

    for (auto* box : staticColliders) {
      accumulateBounds(box);
    }

    if (!hasWorldBounds) {
      worldBounds = SDL_Rect{0,0,Constants::INT_TEN_THOUSAND, Constants::INT_TEN_THOUSAND};
    } else {
      if (worldBounds.w <= 0) worldBounds.w = 1;
      if (worldBounds.h <= 0) worldBounds.h = 1;
    }

    quadtree = Project::Utilities::QuadTree(worldBounds);
    quadtree.clear();

    for (auto* comp : components) {
      if (!comp || !comp->isActive()) continue;
      
      Project::Entities::Entity* owner = comp->getOwner();
      if (!owner) continue;
      
      Project::Components::BoundingBoxComponent* box = owner->getBoundingBoxComponent();
      if (!box) continue;
      
      SDL_Rect bounds{0,0,0,0};
      if (!computeBounds(box, bounds)) continue;
      
      Project::Utilities::Collider collider{box, comp, owner};
      grid.insert(collider, bounds);
      quadtree.insert(collider, bounds);
    }

    for (auto* box : staticColliders) {
      if (!box || !box->isActive()) continue;

      SDL_Rect bounds{0,0,0,0};
      if (!computeBounds(box, bounds)) continue;

      Project::Utilities::Collider collider{box, nullptr, box->getOwner()};
      grid.insert(collider, bounds);
      quadtree.insert(collider, bounds);
    }

    for (auto* comp : components) {
      if (comp && comp->isActive()) {
        comp->update(deltaTime);
      }
    }
  }

  void Project::Systems::PhysicsSystem::clear() {
    components.clear();
    staticColliders.clear();
  }

  SDL_Rect PhysicsSystem::unionRect(const SDL_Rect& a, const SDL_Rect& b) const {
    const int left = std::min(a.x, b.x);
    const int top = std::min(a.y, b.y);
    const int right = std::max(a.x + a.w, b.x + b.w);
    const int bottom = std::max(a.y + a.h, b.y + b.h);
    return {left, top, right - left, bottom - top};
  }

  bool PhysicsSystem::computeBounds(BoundingBoxComponent* box, SDL_Rect& bounds) const {
    if (!box) return false;

    bool hasBounds = false;
    const auto& rects = box->getBoxes();
    for (size_t i = 0; i < rects.size(); ++i) {
      if (!hasBounds) {
        bounds = rects[i];
        hasBounds = true;
      } else {
        const SDL_Rect& r = rects[i];
        const int left = std::min(bounds.x, r.x);
        const int top = std::min(bounds.y, r.y);
        const int right = std::max(bounds.x + bounds.w, r.x + r.w);
        const int bottom = std::max(bounds.y + bounds.h, r.y + r.h);
        bounds = {left, top, right - left, bottom - top};
      }
    }

    const auto& circles = box->getCircles();
    for (const auto& c : circles) {
      SDL_Rect r{
        c.x - c.r, c.y - c.r,
        c.r * Constants::CIRCLE_DIAMETER_MULTIPLIER,
        c.r * Constants::CIRCLE_DIAMETER_MULTIPLIER
      };
      
      if (!hasBounds) {
        bounds = r;
        hasBounds = true;
      } else {
        const int left = std::min(bounds.x, r.x);
        const int top = std::min(bounds.y, r.y);
        const int right = std::max(bounds.x + bounds.w, r.x + r.w);
        const int bottom = std::max(bounds.y + bounds.h, r.y + r.h);
        bounds = {left, top, right - left, bottom - top};
      }
    }

    const auto& polys = box->getPolygons();
    for (const auto& p : polys) {
      SDL_Rect r = Project::Utilities::GeometryUtils::polygonBounds(p);
      if (!hasBounds) {
        bounds = r;
        hasBounds = true;
      } else {
        const int left = std::min(bounds.x, r.x);
        const int top = std::min(bounds.y, r.y);
        const int right = std::max(bounds.x + bounds.w, r.x + r.w);
        const int bottom = std::max(bounds.y + bounds.h, r.y + r.h);
        bounds = {left, top, right - left, bottom - top};
      }
    }

    const auto& caps = box->getCapsules();
    for (const auto& c : caps) {
      SDL_Rect r = Project::Utilities::GeometryUtils::capsuleBounds(c);
      if (!hasBounds) {
        bounds = r;
        hasBounds = true;
      } else {
        const int left = std::min(bounds.x, r.x);
        const int top = std::min(bounds.y, r.y);
        const int right = std::max(bounds.x + bounds.w, r.x + r.w);
        const int bottom = std::max(bounds.y + bounds.h, r.y + r.h);
        bounds = {left, top, right - left, bottom - top};
      }
    }

    return hasBounds;
  }
}
