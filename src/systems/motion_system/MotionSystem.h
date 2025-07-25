#ifndef MOTION_SYSTEM_H
#define MOTION_SYSTEM_H

#include <vector>
#include "interfaces/update_interface/Updatable.h"

namespace Project { namespace Components { class MotionComponent; } }

namespace Project::Systems {
  class MotionSystem : public Project::Interfaces::Updatable {
  public:
    void add(Project::Components::MotionComponent* component);
    void remove(Project::Components::MotionComponent* component);
    void clear();
    void update(float deltaTime) override;
  
    private:
    std::vector<Project::Components::MotionComponent*> components;
  };
}

#endif
