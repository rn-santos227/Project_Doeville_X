#ifndef MOVEMENT_MODE_RESOLVER_H
#define MOVEMENT_MODE_RESOLVER_H

#include "MovementMode.h"
#include <string>

namespace Project::Components {
  class SurfaceTypeResolver {
  public:
    static MovementMode resolve(const std::string& name);
  };
}

#endif
