#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <vector>
#include "interfaces/update_interface/Updatable.h"
#include "utilities/spatial/SpatialHashGrid.h"

namespace Project { namespace Components { class PhysicsComponent; } }

namespace Project::Systems {
  class PhysicsSystem : public Project::Interfaces::Updatable {
  public:
    void add(Project::Components::PhysicsComponent* component);
    void remove(Project::Components::PhysicsComponent* component);

    void update(float deltaTime) override;
    void clear();

    const Project::Utilities::SpatialHashGrid& getGrid() const { return grid; }
    Project::Utilities::SpatialHashGrid& getGrid() { return grid; }

  private:
    std::vector<Project::Components::PhysicsComponent*> components;
    Project::Utilities::SpatialHashGrid grid;

  };
}

#endif
