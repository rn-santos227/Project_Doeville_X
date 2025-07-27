#ifndef SURFACE_TYPE_RESOLVER_H
#define SURFACE_TYPE_RESOLVER_H

#include "SurfaceType.h"
#include <string>

namespace Project::Components {
  class SurfaceTypeResolver {
  public:
    static SurfaceType resolve(const std::string& name);
  };
}

#endif
