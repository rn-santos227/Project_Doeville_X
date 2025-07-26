#include "PhysicsSystem.h"

#include <algorithm>

#include "entities/Entity.h"
#include "components/physics_component/PhysicsComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"

namespace Project::Systems {
  using Project::Components::PhysicsComponent;

  void Project::Systems::PhysicsSystem::add(PhysicsComponent* component) {
    if (component) components.push_back(component);
  }

  void Project::Systems::PhysicsSystem::remove(PhysicsComponent* component) {
    components.erase(std::remove(components.begin(), components.end(), component), components.end());
  }

  void Project::Systems::PhysicsSystem::update(float deltaTime) {
    grid.clear();
    for (auto* comp : components) {
      if (!comp || !comp->isActive()) continue;
      Project::Entities::Entity* owner = comp->getOwner();
      if (!owner) continue;
      Project::Components::BoundingBoxComponent* box = owner->getBoundingBoxComponent();
      if (!box) continue;
      const auto& rects = box->getBoxes();
      if (rects.empty()) continue;
      SDL_Rect bounds = rects[0];
      for (size_t i = 1; i < rects.size(); ++i) {
        const SDL_Rect& r = rects[i];
        const int left = std::min(bounds.x, r.x);
        const int top = std::min(bounds.y, r.y);
        const int right = std::max(bounds.x + bounds.w, r.x + r.w);
        const int bottom = std::max(bounds.y + bounds.h, r.y + r.h);
        bounds = {left, top, right - left, bottom - top};
      }
      grid.insert(comp, bounds);
    }

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
