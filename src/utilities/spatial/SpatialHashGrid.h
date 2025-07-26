#ifndef SPATIAL_HASH_GRID_H
#define SPATIAL_HASH_GRID_H

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <SDL.h>

#include "components/physics_component/PhysicsComponent.h"
#include "libraries/constants/FloatConstants.h"

namespace Project { namespace Components { class PhysicsComponent; } }

namespace Project::Utilities {
  class SpatialHashGrid {
  public:
    explicit SpatialHashGrid(float cellSize = Project::Libraries::Constants::DEFAULT_CELL_SIZE);
    
    std::vector<Project::Components::PhysicsComponent*> query(const SDL_Rect& area) const;
    void insert(Project::Components::PhysicsComponent* obj, const SDL_Rect& bounds);
    void clear();

    float getCellSize() const { return cellSize; }

  private:
    using Cell = std::vector<Project::Components::PhysicsComponent*>;
    std::unordered_map<long long, Cell> cells;

    float cellSize;
    long long hash(int x, int y) const;
  };
}

#endif
