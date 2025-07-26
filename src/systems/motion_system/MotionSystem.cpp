#include "MotionSystem.h"
#include <algorithm>
#include "components/motion_component/MotionComponent.h"
#include "libraries/constants/NumericConstants.h"

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
