#include "RenderSystem.h"

#include <algorithm>

#include "components/graphics_component/GraphicsComponent.h"
#include "libraries/constants/NumericConstants.h"
#include "libraries/constants/ProfileConstants.h"
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
      GPU_PROFILE_SCOPE(Constants::RENDER_SCOPE);
      drawBuffer(commandBuffers[readIndex]);
    }

    pool.enqueue([](){ GPU_PROFILE_SCOPE(Constants::POST_PROCESS_SCOPE); });
    pool.enqueue([](){ GPU_PROFILE_SCOPE(Constants::PARTICLE_SCOPE); });

    prepareCommandBuffer(commandBuffers[writeIndex]);
    std::swap(readIndex, writeIndex);
    ++currentFrame;
  }

  void RenderSystem::clear() {
    components.clear();
    commandBuffers[0].clear();
    commandBuffers[1].clear();
    firstFrame = true;
    currentFrame = 0;
  }

  bool RenderSystem::rectContains(const SDL_FRect& outer, const SDL_FRect& inner) const {
    return inner.x >= outer.x && inner.y >= outer.y && inner.x + inner.w <= outer.x + outer.w && inner.y + inner.h <= outer.y + outer.h;
  }

  void RenderSystem::prepareCommandBuffer(std::vector<GraphicsComponent*>& buffer) {
    buffer.clear();
    for (auto* comp : components) {
      if (comp && comp->isActive()) buffer.push_back(comp);
    }
    std::sort(buffer.begin(), buffer.end(), [](GraphicsComponent* a, GraphicsComponent* b) {
      const auto& boxA = a->getBoundingBox();
      const auto& boxB = b->getBoundingBox();

      const float bottomA = boxA.y + boxA.h;
      const float bottomB = boxB.y + boxB.h;
      if (bottomA != bottomB) return bottomA < bottomB;

      if (a->getMaterialId() != b->getMaterialId()) return a->getMaterialId() < b->getMaterialId();
      if (a->getBlendMode() != b->getBlendMode()) return a->getBlendMode() < b->getBlendMode();
      return a->getBatchTexture() < b->getBatchTexture();
    });
  }

  void RenderSystem::drawBuffer(const std::vector<GraphicsComponent*>& buffer) {
    for (auto* comp : buffer) {
      if (comp && comp->isActive()) {
        comp->render();
        Project::Utilities::Profiler::getInstance().incrementDrawCalls();
      }
    }
  }
}
