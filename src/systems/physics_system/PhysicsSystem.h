#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <vector>
#include "interfaces/update_interface/Updatable.h"
#include "utilities/spatial/SpatialHashGrid.h"

namespace Project { namespace Components { class PhysicsComponent; } }

namespace Project::Systems {
  class PhysicsSystem : public Project::Interfaces::Updatable {
  public:
    PhysicsSystem();

    void add(Project::Components::PhysicsComponent* component);
    void remove(Project::Components::PhysicsComponent* component);

    void addStaticCollider(Project::Components::BoundingBoxComponent* box);
    void removeStaticCollider(Project::Components::BoundingBoxComponent* box);

    void update(float deltaTime) override;
    void clear();

    const Project::Utilities::SpatialHashGrid& getGrid() const { return grid; }
    Project::Utilities::SpatialHashGrid& getGrid() { return grid; }

  private:
    Project::Utilities::SpatialHashGrid grid;
    
    std::vector<Project::Components::PhysicsComponent*> components;
    std::vector<Project::Components::BoundingBoxComponent*> staticColliders;

  };
}

#endif
