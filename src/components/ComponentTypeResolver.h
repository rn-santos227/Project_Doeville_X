#ifndef COMPONENT_TYPE_RESOLVER_H
#define COMPONENT_TYPE_RESOLVER_H

#include "ComponentType.h"

#include <string_view>

namespace Project::Components {
  class ComponentTypeResolver {
  public:
    static ComponentType resolve(std::string_view name);
  };
}

#endif
