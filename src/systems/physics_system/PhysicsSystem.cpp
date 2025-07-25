#include "PhysicsSystem.h"
#include <algorithm>
#include "components/physics_component/PhysicsComponent.h"

namespace Project::Systems {
  using Project::Components::PhysicsComponent;

  void Project::Systems::PhysicsSystem::add(PhysicsComponent* component) {
    if (component) components.push_back(component);
  }

  void Project::Systems::PhysicsSystem::remove(PhysicsComponent* component) {
    components.erase(std::remove(components.begin(), components.end(), component), components.end());
  }

  void Project::Systems::PhysicsSystem::update(float deltaTime) {
    for (auto* comp : components) {
      if (comp && comp->isActive()) {
        comp->update(deltaTime);
      }
    }
  }

  void Project::Systems::PhysicsSystem::clear() {
    components.clear();
  }
}
