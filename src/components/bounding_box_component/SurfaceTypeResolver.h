#ifndef SURFACE_TYPE_RESOLVER_H
#define SURFACE_TYPE_RESOLVER_H

#include "SurfaceType.h"
#include <string_view>

namespace Project::Components {
  class SurfaceTypeResolver {
  public:
    static SurfaceType resolve(std::string_view name);
  };
}

#endif
