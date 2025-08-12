#include "MotionSystem.h"
#include <algorithm>
#include "components/motion_component/MotionComponent.h"
#include "libraries/constants/NumericConstants.h"
#include "libraries/constants/ProfileConstants.h"
#include "utilities/profiler/Profiler.h"
#include "utilities/thread/ThreadPool.h"

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
    auto& pool = Project::Utilities::ThreadPool::getInstance();
    const size_t chunk = 64;
    const size_t total = components.size();
    for (size_t i = 0; i < total; i += chunk) {
      size_t begin = i;
      size_t end = std::min(i + chunk, total);
      pool.enqueue([this, begin, end, deltaTime]() {
        for (size_t j = begin; j < end; ++j) {
          auto* comp = components[j];
          if (comp && comp->isActive()) {
            comp->update(deltaTime);
          }
        }
      });
    }
    pool.wait();
  }

  void Project::Systems::MotionSystem::clear() {
    components.clear();
  }
}
