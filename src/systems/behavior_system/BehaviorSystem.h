#ifndef BEHAVIOR_SYSTEM_H
#define BEHAVIOR_SYSTEM_H

#include "interfaces/update_interface/Updatable.h"
#include "utilities/profiler/Profiler.h"
#include "libraries/constants/ProfileConstants.h"

namespace Project::Systems {
  class BehaviorSystem : public Project::Interfaces::Updatable {
  public:
    void update(float deltaTime) override;
  };
}

#endif
