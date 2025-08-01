#ifndef ENTITY_ATTRIBUTE_RESOLVER_H
#define ENTITY_ATTRIBUTE_RESOLVER_H

#include "EntityAttribute.h"
#include <string_view>

namespace Project::Entities {
  class EntityAttributeResolver {
  public:
    static EntityAttribute resolve(std::string_view name);
  };
}

#endif
