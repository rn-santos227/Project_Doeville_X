#ifndef STATIC_COLLISION_BEHAVIOR_RESOLVER
#define STATIC_COLLISION_BEHAVIOR_RESOLVER

#include "StaticCollisionBehavior.h"
#include <string>

namespace Project::Components::Physics {
  class StaticCollisionBehaviorResolver {
  public:
    static StaticCollisionBehavior resolve(const std::string& name);
  };
}

#endif
