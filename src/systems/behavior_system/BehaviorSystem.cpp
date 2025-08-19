
#include "BehaviorSystem.h"

#include <algorithm>

#include "components/behavior_component/BehaviorComponent.h"
#include "libraries/constants/ProfileConstants.h"
#include "utilities/profiler/Profiler.h"

namespace Project::Systems {
  using Project::Components::BehaviorComponent;

  void BehaviorSystem::update(float deltaTime) {
    PROFILE_SCOPE(Project::Libraries::Constants::BEHAVIOR_PROFILE);
    for (auto* comp : components) {
      if (comp && comp->isActive()) {
        comp->update(deltaTime);
      }
    }
  }

  void BehaviorSystem::add(BehaviorComponent* component) {
    if (component) components.push_back(component);
  }

  void BehaviorSystem::remove(BehaviorComponent* component) {
    components.erase(std::remove(components.begin(), components.end(), component), components.end());
  }

  void BehaviorSystem::clear() {
    components.clear();
  }
}
