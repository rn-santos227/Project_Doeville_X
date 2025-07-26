#include "SpatialHashGrid.h"

#include <cmath>

#include "libraries/constants/NumericConstants.h"

namespace Project::Utilities {
  using Project::Components::PhysicsComponent;
  SpatialHashGrid::SpatialHashGrid(float cellSize) : cellSize(cellSize) {}

  std::vector<PhysicsComponent*> SpatialHashGrid::query(const SDL_Rect& area) const {
    std::vector<PhysicsComponent*> result;
    std::unordered_set<PhysicsComponent*> seen;
    int minX = static_cast<int>(std::floor(static_cast<float>(area.x) / cellSize));
    int minY = static_cast<int>(std::floor(static_cast<float>(area.y) / cellSize));
    int maxX = static_cast<int>(std::floor(static_cast<float>(area.x + area.w) / cellSize));
    int maxY = static_cast<int>(std::floor(static_cast<float>(area.y + area.h) / cellSize));

    for (int x = minX; x <= maxX; ++x) {
      for (int y = minY; y <= maxY; ++y) {
        auto it = cells.find(hash(x, y));
        if (it != cells.end()) {
          for (auto* obj : it->second) {
            if (seen.insert(obj).second) {
                result.push_back(obj);
            }
          }
        }
      }
    }

    return result;
  }

  void SpatialHashGrid::insert(PhysicsComponent* obj, const SDL_Rect& bounds) {
    int minX = static_cast<int>(std::floor(static_cast<float>(bounds.x) / cellSize));
    int minY = static_cast<int>(std::floor(static_cast<float>(bounds.y) / cellSize));
    int maxX = static_cast<int>(std::floor(static_cast<float>(bounds.x + bounds.w) / cellSize));
    int maxY = static_cast<int>(std::floor(static_cast<float>(bounds.y + bounds.h) / cellSize));
    for (int x = minX; x <= maxX; ++x) {
      for (int y = minY; y <= maxY; ++y) {
        cells[hash(x, y)].push_back(obj);
      }
    }
  }

  void SpatialHashGrid::clear() {
    cells.clear();
  }

  long long SpatialHashGrid::hash(int x, int y) const {
    return (static_cast<long long>(x) << Project::Libraries::Constants::BIT_SHIFT) ^ static_cast<unsigned long long>(y);
  }
}
