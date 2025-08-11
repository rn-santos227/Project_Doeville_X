#include "PhysicsSystem.h"

#include <cmath>
#include <algorithm>
#include <limits>

#include "components/physics_component/PhysicsData.h"
#include "components/physics_component/PhysicsComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "entities/Entity.h"
#include "libraries/constants/Constants.h"
#include "utilities/geometry/GeometryUtils.h"
#include "utilities/profiler/Profiler.h"
#include "utilities/thread/ThreadPool.h"

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
    PROFILE_SCOPE(Constants::PHYSICS_PROFILE);
    grid.clear();
    highPriorityGrid.clear();
    lowPriorityGrid.clear();

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

    auto& pool = Project::Utilities::ThreadPool::getInstance();
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

    const size_t totalColliders = components.size() + staticColliders.size();
    const float area = static_cast<float>(worldBounds.w) * static_cast<float>(worldBounds.h);
    float targetCell = std::sqrt(area / (static_cast<float>(totalColliders) + Constants::DEFAULT_WHOLE));
    targetCell = std::clamp(targetCell, Constants::MIN_CELL, Constants::MAX_CELL);
    
    grid.setCellSize(targetCell);
    highPriorityGrid.setCellSize(targetCell);
    lowPriorityGrid.setCellSize(targetCell);

    quadtree = Project::Utilities::QuadTree(worldBounds);
    quadtree.clear();

    std::vector<std::pair<SDL_Rect, Project::Utilities::Collider>> dynamicObjects;

    for (auto* comp : components) {
      if (!comp || !comp->isActive()) continue;
      
      Project::Entities::Entity* owner = comp->getOwner();
      if (!owner) continue;
      
      Project::Components::BoundingBoxComponent* box = owner->getBoundingBoxComponent();
      if (!box) continue;
      
      SDL_Rect bounds{0,0,0,0};
      if (!computeBounds(box, bounds)) continue;
      
      Project::Utilities::Collider collider{box, comp, owner};
      switch (comp->getUpdateFrequency()) {
        case Project::Components::UpdateFrequency::HIGH:
          highPriorityGrid.insert(collider, bounds);
          break;
        case Project::Components::UpdateFrequency::LOW:
          lowPriorityGrid.insert(collider, bounds);
          break;
        default:
          grid.insert(collider, bounds);
          break;
      }
      
      quadtree.insert(collider, bounds);
      dynamicObjects.emplace_back(bounds, collider);

      auto& catGrid = categoryGrids[owner->getEntityCategory()];
      catGrid.setCellSize(targetCell);
      catGrid.insert(collider, bounds);
    }

    for (auto* box : staticColliders) {
      if (!box || !box->isActive()) continue;

      SDL_Rect bounds{0,0,0,0};
      if (!computeBounds(box, bounds)) continue;

      Project::Utilities::Collider collider{box, nullptr, box->getOwner()};
      quadtree.insert(collider, bounds);
      if (collider.entity) {
        auto& catGrid = categoryGrids[collider.entity->getEntityCategory()];
        catGrid.setCellSize(targetCell);
        catGrid.insert(collider, bounds);
      }
    }

    auto start = std::chrono::high_resolution_clock::now();
    sweepPairs = Project::Utilities::SweepAndPrune::findPairs(dynamicObjects);
    auto end = std::chrono::high_resolution_clock::now();
    metrics.lastBroadPhaseMs = std::chrono::duration<float, std::milli>(end - start).count();

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

  void Project::Systems::PhysicsSystem::recordSpatialQuery(float ms) {
    metrics.queryCount++;
    metrics.totalQueryTimeMs += ms;
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
