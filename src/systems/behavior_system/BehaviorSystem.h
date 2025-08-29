#ifndef BEHAVIOR_SYSTEM_H
#define BEHAVIOR_SYSTEM_H

#include <vector>

#include "interfaces/update_interface/Updatable.h"

namespace Project { namespace Components { class BehaviorComponent; } }

namespace Project::Systems {
  class BehaviorSystem : public Project::Interfaces::Updatable {
  public:
    void update(float deltaTime) override;
    
    void add(Project::Components::BehaviorComponent* component);
    void remove(Project::Components::BehaviorComponent* component);
    void clear();

  private:
    std::vector<Project::Components::BehaviorComponent*> components;
  };
}

#endif
