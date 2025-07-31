#ifndef COMPONENT_TYPE_RESOLVER_H
#define COMPONENT_TYPE_RESOLVER_H

#include "ComponentType.h"

#include <string>
#include <unordered_map>

namespace Project::Components {
  class ComponentTypeResolver {
  public:
    static ComponentType resolve(const std::string& name);
  };
}

#endif
