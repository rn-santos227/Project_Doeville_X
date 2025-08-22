#include "RenderSystem.h"

#include <algorithm>

#include "components/graphics_component/GraphicsComponent.h"
#include "libraries/constants/Constants.h"
#include "utilities/profiler/Profiler.h"
#include "utilities/thread/ThreadPool.h"

namespace Project::Systems {
  using Project::Components::GraphicsComponent;

  namespace Constants = Project::Libraries::Constants;
  RenderSystem::RenderSystem() {
    components.reserve(Constants::MAX_MEMORY_SPACE);
  }

  void RenderSystem::add(GraphicsComponent* component) {
    if (component) components.push_back(component);
  }

  void RenderSystem::remove(GraphicsComponent* component) {
    components.erase(std::remove(components.begin(), components.end(), component), components.end());
  }

  void RenderSystem::update(float deltaTime) {
    PROFILE_SCOPE(Project::Libraries::Constants::RENDER_PROFILE);
    for (auto* comp : components) {
      if (comp && comp->isActive()) comp->update(deltaTime);
    }
  }

  void RenderSystem::render() {
    auto& pool = Project::Utilities::ThreadPool::getInstance();
    if (firstFrame) {
      prepareCommandBuffer(commandBuffers[readIndex]);
      firstFrame = false;
    }

    {
      GPU_PROFILE_SCOPE("render");
      drawBuffer(commandBuffers[readIndex]);
    }
  }

  void Project::Systems::RenderSystem::clear() {
    components.clear();
  }

  bool RenderSystem::rectContains(const SDL_FRect& outer, const SDL_FRect& inner) const {
    return inner.x >= outer.x && inner.y >= outer.y && (inner.x + inner.w) <= (outer.x + outer.w) && (inner.y + inner.h) <= (outer.y + outer.h);
  }
}
