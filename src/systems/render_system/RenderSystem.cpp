#include "RenderSystem.h"

#include <algorithm>

#include "components/graphics_component/GraphicsComponent.h"

namespace Project::Systems {
  using Project::Components::GraphicsComponent;

  void Project::Systems::RenderSystem::add(GraphicsComponent* component) {
    if (component) components.push_back(component);
  }

  void Project::Systems::RenderSystem::remove(GraphicsComponent* component) {
    components.erase(std::remove(components.begin(), components.end(), component), components.end());
  }

  void Project::Systems::RenderSystem::update(float deltaTime) {
    for (auto* comp : components) {
      if (comp && comp->isActive()) {
        comp->update(deltaTime);
      }
    }
  }

  void Project::Systems::RenderSystem::render() {
    for (auto* comp : components) {
      if (comp && comp->isActive()) {
        comp->render();
      }
    }
  }

  void Project::Systems::RenderSystem::clear() {
    components.clear();
  }
}
