#ifndef SPATIAL_HASH_GRID_H
#define SPATIAL_HASH_GRID_H

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <SDL.h>

#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "entities/Entity.h"
#include "libraries/constants/FloatConstants.h"

namespace Project { namespace Components { class PhysicsComponent; } }

namespace Project::Utilities {
  struct Collider {
    Project::Components::BoundingBoxComponent* box = nullptr;
    Project::Components::PhysicsComponent* physics = nullptr;
    Project::Entities::Entity* entity = nullptr;
  };

  class SpatialHashGrid {
  public:
    explicit SpatialHashGrid(float cellSize = Project::Libraries::Constants::DEFAULT_CELL_SIZE);
    
    std::vector<Collider> query(const SDL_Rect& area) const;
    void insert(const Collider& obj, const SDL_Rect& bounds);
    void clear();

    float getCellSize() const { return cellSize; }
    void setCellSize(float newSize);

  private:
    using Cell = std::vector<Collider>;
    std::unordered_map<long long, Cell> cells;

    float cellSize;
    long long hash(int x, int y) const;
  };
}

#endif
