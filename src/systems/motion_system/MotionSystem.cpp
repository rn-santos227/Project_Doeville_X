#include "MotionSystem.h"
#include <algorithm>
#include "components/motion_component/MotionComponent.h"
#include "libraries/constants/NumericConstants.h"
#include "libraries/constants/ProfileConstants.h"
#include "utilities/profiler/Profiler.h"

namespace Project::Systems {
  using Project::Components::MotionComponent;

  MotionSystem::MotionSystem() {
    components.reserve(Project::Libraries::Constants::INT_HUNDRED);
  }

  void Project::Systems::MotionSystem::add(MotionComponent* component) {
    if (component) components.push_back(component);
  }

  void Project::Systems::MotionSystem::remove(MotionComponent* component) {
    components.erase(std::remove(components.begin(), components.end(), component), components.end());
  }

  void Project::Systems::MotionSystem::update(float deltaTime) {
    PROFILE_SCOPE(Project::Libraries::Constants::MOTION_PROFILE);
    for (auto* comp : components) {
      if (comp && comp->isActive()) {
        comp->update(deltaTime);
      }
    }
  }

  void Project::Systems::MotionSystem::clear() {
    components.clear();
  }
}
