#ifndef STATIC_COLLISION_BEHAVIOR_RESOLVER
#define STATIC_COLLISION_BEHAVIOR_RESOLVER

#include "SurfaceType.h"
#include <string>

namespace Project::Components::Physics {
  class SurfaceTypeResolver {
  public:
    static SurfaceType resolve(const std::string& name);
  };
}

#endif
