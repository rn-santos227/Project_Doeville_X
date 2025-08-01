#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <chrono>
#include <unordered_map>
#include <vector>

#include "interfaces/update_interface/Updatable.h"
#include "utilities/spatial/QuadTree.h"
#include "entities/EntityCategory.h"
#include "utilities/spatial/SweepAndPrune.h"

namespace Project { namespace Components { class PhysicsComponent; } }

namespace Project::Systems {
  class PhysicsSystem : public Project::Interfaces::Updatable {
  public:
    struct PerformanceMetrics {
      std::size_t queryCount = 0;
      float totalQueryTimeMs = 0.0f;
      float lastBroadPhaseMs = 0.0f;
    };

    PhysicsSystem();

    void add(Project::Components::PhysicsComponent* component);
    void remove(Project::Components::PhysicsComponent* component);

    void addStaticCollider(Project::Components::BoundingBoxComponent* box);
    void removeStaticCollider(Project::Components::BoundingBoxComponent* box);

    void update(float deltaTime) override;
    void clear();

    const Project::Utilities::SpatialHashGrid& getGrid() const { return grid; }
    Project::Utilities::SpatialHashGrid& getGrid() { return grid; }
    const Project::Utilities::SpatialHashGrid& getHighPriorityGrid() const { return highPriorityGrid; }
    Project::Utilities::SpatialHashGrid& getHighPriorityGrid() { return highPriorityGrid; }
    const Project::Utilities::SpatialHashGrid& getLowPriorityGrid() const { return lowPriorityGrid; }
    Project::Utilities::SpatialHashGrid& getLowPriorityGrid() { return lowPriorityGrid; }
    const Project::Utilities::QuadTree& getQuadTree() const { return quadtree; }
    Project::Utilities::QuadTree& getQuadTree() { return quadtree; }
    const std::vector<Project::Utilities::SweepAndPrune::Pair>& getSweepPairs() const { return sweepPairs; }

    const PerformanceMetrics& getPerformanceMetrics() const { return metrics; }
    void recordSpatialQuery(float ms);

  private:
    PerformanceMetrics metrics;
    Project::Utilities::QuadTree quadtree;
    
    Project::Utilities::SpatialHashGrid grid;
    Project::Utilities::SpatialHashGrid highPriorityGrid;
    Project::Utilities::SpatialHashGrid lowPriorityGrid;
    std::unordered_map<Project::Entities::EntityCategory, Project::Utilities::SpatialHashGrid> categoryGrids;

    std::vector<Project::Utilities::SweepAndPrune::Pair> sweepPairs;
    
    std::vector<Project::Components::PhysicsComponent*> components;
    std::vector<Project::Components::BoundingBoxComponent*> staticColliders;

    SDL_Rect unionRect(const SDL_Rect& a, const SDL_Rect& b) const;
    bool computeBounds(Project::Components::BoundingBoxComponent* box, SDL_Rect& bounds) const;
    void resetMetrics() { metrics.queryCount = 0; metrics.totalQueryTimeMs = 0.0f; metrics.lastBroadPhaseMs = 0.0f; }
  };
}

#endif
