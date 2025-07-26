#include "RenderSystem.h"
#include <algorithm>
#include "components/graphics_component/GraphicsComponent.h"
#include "handlers/camera/CameraHandler.h"
#include "libraries/constants/NumericConstants.h"

namespace Project::Systems {
  using Project::Components::GraphicsComponent;

  RenderSystem::RenderSystem() {
    components.reserve(Project::Libraries::Constants::MAX_MEMORY_SPACE);
  }

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
    std::vector<SDL_Rect> drawn;
    drawn.reserve(components.size());
    auto* camHandler = GraphicsComponent::getCameraHandler();
    SDL_Rect cullRect{0, 0, 0, 0};
    bool useCull = false;
    if (camHandler) {
      cullRect = camHandler->getCullingRect();
      useCull = true;
    }

    for (auto* comp : components) {
      if (!comp || !comp->isActive()) continue;
      const SDL_Rect rect = comp->getRect();

      if (useCull && !SDL_HasIntersection(&rect, &cullRect)) {
        continue;
      }

      bool occluded = false;
      for (const auto& r : drawn) {
        if (rectContains(r, rect)) {
          occluded = true;
          break;
        }
      }
      if (occluded) continue;

      comp->render();
      drawn.push_back(rect);
    }
  }

  void Project::Systems::RenderSystem::clear() {
    components.clear();
  }

  bool RenderSystem::rectContains(const SDL_Rect& outer, const SDL_Rect& inner) const {
    return inner.x >= outer.x && inner.y >= outer.y &&
           (inner.x + inner.w) <= (outer.x + outer.w) &&
           (inner.y + inner.h) <= (outer.y + outer.h);
  }
}
