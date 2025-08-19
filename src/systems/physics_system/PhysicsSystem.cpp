#include "PhysicsSystem.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <utility>

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
  : quadtree(SDL_FRect{0.f, 0.f, static_cast<float>(Constants::INT_TEN_THOUSAND), static_cast<float>(Constants::INT_TEN_THOUSAND)}) {
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

    SDL_FRect worldBounds{0.f, 0.f, 0.f, 0.f};
    bool hasWorldBounds = false;

    auto accumulateBounds = [&](BoundingBoxComponent* box) {
      if (!box || !box->isActive()) return;
      SDL_FRect b{0.f, 0.f, 0.f, 0.f};
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
      worldBounds = SDL_FRect{
        0.f, 0.f,
        static_cast<float>(Constants::INT_TEN_THOUSAND),
        static_cast<float>(Constants::INT_TEN_THOUSAND)
      };
    } else {
      if (worldBounds.w <= 0.f) worldBounds.w = 1.f;
      if (worldBounds.h <= 0.f) worldBounds.h = 1.f;
    }

    const size_t totalColliders = components.size() + staticColliders.size();
    const float area = worldBounds.w * worldBounds.h;
    float targetCell = std::sqrt(area / (static_cast<float>(totalColliders) + Constants::DEFAULT_WHOLE));
    targetCell = std::clamp(targetCell, Constants::MIN_CELL, Constants::MAX_CELL);
    
    grid.setCellSize(targetCell);
    highPriorityGrid.setCellSize(targetCell);
    lowPriorityGrid.setCellSize(targetCell);

    quadtree = Project::Utilities::QuadTree(worldBounds);
    quadtree.clear();
    bvh.clear();

    std::vector<std::pair<SDL_FRect, Project::Utilities::Collider>> dynamicObjects;
    dynamicObjects.reserve(components.size());
    std::vector<std::pair<SDL_FRect, Project::Utilities::Collider>> allObjects;
    allObjects.reserve(components.size() + staticColliders.size());

    for (auto* comp : components) {
      if (!comp || !comp->isActive()) continue;
      
      Project::Entities::Entity* owner = comp->getOwner();
      if (!owner) continue;
      
      Project::Components::BoundingBoxComponent* box = owner->getBoundingBoxComponent();
      if (!box) continue;
      
      SDL_FRect fBounds{0.f,0.f,0.f,0.f};
      if (!computeBounds(box, fBounds)) continue;
      SDL_Rect bounds{
        static_cast<int>(std::floor(fBounds.x)),
        static_cast<int>(std::floor(fBounds.y)),
        static_cast<int>(std::ceil(fBounds.w)),
        static_cast<int>(std::ceil(fBounds.h))
      };
      
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
      
      quadtree.insert(collider, fBounds);
      dynamicObjects.emplace_back(fBounds, collider);
      allObjects.emplace_back(fBounds, collider);

      float centerX = worldBounds.x + worldBounds.w * Constants::CENTER_FACTOR;
      float centerY = worldBounds.y + worldBounds.h * Constants::CENTER_FACTOR;
      float dx = owner->getX() - centerX;
      float dy = owner->getY() - centerY;
      float dist = std::sqrt(dx * dx + dy * dy);
      float tick = Constants::HIGH_TICK_RATE;
      switch (comp->getUpdateFrequency()) {
        case Project::Components::UpdateFrequency::LOW:
          tick = Constants::LOW_TICK_RATE;
          break;
        default:
          tick = Constants::HIGH_TICK_RATE;
          break;
      }
      if (dist > Constants::FAR_DISTANCE_THRESHOLD) {
        tick = (tick > Constants::HIGH_TICK_RATE ? tick : Constants::DEFAULT_TICK_RATE) * Constants::FAR_TICK_MULTIPLIER;
      } else if (dist > Constants::MID_DISTANCE_THRESHOLD) {
        tick = (tick > Constants::HIGH_TICK_RATE ? tick : Constants::DEFAULT_TICK_RATE) * Constants::MID_TICK_MULTIPLIER;
      }
      comp->setTickRate(tick);

      auto& catGrid = categoryGrids[owner->getEntityCategory()];
      catGrid.setCellSize(targetCell);
      catGrid.insert(collider, bounds);
    }

    for (auto* box : staticColliders) {
      if (!box || !box->isActive()) continue;

      SDL_FRect fBounds{0.f, 0.f, 0.f, 0.f};
      if (!computeBounds(box, fBounds)) continue;
      SDL_Rect bounds{
        static_cast<int>(std::floor(fBounds.x)),
        static_cast<int>(std::floor(fBounds.y)),
        static_cast<int>(std::ceil(fBounds.w)),
        static_cast<int>(std::ceil(fBounds.h))
      };

      Project::Utilities::Collider collider{box, nullptr, box->getOwner()};
      quadtree.insert(collider, fBounds);
      allObjects.emplace_back(fBounds, collider);
      if (collider.entity) {
        auto& catGrid = categoryGrids[collider.entity->getEntityCategory()];
        catGrid.setCellSize(targetCell);
        catGrid.insert(collider, bounds);
      }
    }

    auto start = std::chrono::high_resolution_clock::now();
    sweepPairs = Project::Utilities::SweepAndPrune::findPairs(dynamicObjects);
    sweepPairKeys.clear();
    sweepPairKeys.reserve(sweepPairs.size());
    auto makeKey = [](PhysicsComponent* a, PhysicsComponent* b) {
      auto pa = reinterpret_cast<std::uintptr_t>(a);
      auto pb = reinterpret_cast<std::uintptr_t>(b);
      if (pa > pb) std::swap(pa, pb);
      return static_cast<std::size_t>(pa ^ (pb + Constants::DEFAULT_HASH + (pa << 6) + (pa >> 2)));
    };
    for (const auto& p : sweepPairs) {
      sweepPairKeys.insert(makeKey(p.first.physics, p.second.physics));
    }
    
    bvh.build(allObjects);
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

  SDL_FRect PhysicsSystem::unionRect(const SDL_FRect& a, const SDL_FRect& b) const {
    const float left = std::min(a.x, b.x);
    const float top = std::min(a.y, b.y);
    const float right = std::max(a.x + a.w, b.x + b.w);
    const float bottom = std::max(a.y + a.h, b.y + b.h);
    return {left, top, right - left, bottom - top};
  }

  bool PhysicsSystem::computeBounds(BoundingBoxComponent* box, SDL_FRect& bounds) const {
    if (!box) return false;

    if (box->usesProxy()) {
      bounds = box->getProxyAABB();
      return true;
    }

    bool hasBounds = false;
    const auto& rects = box->getBoxes();
    for (const auto& fr : rects) {
      if (!hasBounds) {
        bounds = fr;
        hasBounds = true;
      } else {
        bounds = unionRect(bounds, fr);
      }
    }

    const auto& circles = box->getCircles();
    for (const auto& c : circles) {
      SDL_FRect r{
        c.x - c.r,
        c.y - c.r,
        c.r * Constants::CIRCLE_DIAMETER_MULTIPLIER,
        c.r * Constants::CIRCLE_DIAMETER_MULTIPLIER
      };
      
      if (!hasBounds) {
        bounds = r;
        hasBounds = true;
      } else {
        bounds = unionRect(bounds, r);
      }
    }

    const auto& polys = box->getPolygons();
    for (const auto& p : polys) {
      SDL_FRect fr = Project::Utilities::GeometryUtils::polygonBounds(p);
      if (!hasBounds) {
        bounds = fr;
        hasBounds = true;
      } else {
        bounds = unionRect(bounds, fr);
      }
    }

    const auto& caps = box->getCapsules();
    for (const auto& c : caps) {
      SDL_FRect fr = Project::Utilities::GeometryUtils::capsuleBounds(c);
      if (!hasBounds) {
        bounds = fr;
        hasBounds = true;
      } else {
        bounds = unionRect(bounds, fr);
      }
    }
    return hasBounds;
  }
}
