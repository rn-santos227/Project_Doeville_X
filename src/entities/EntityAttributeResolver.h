#ifndef ENTITY_ATTRIBUTE_RESOLVER_H
#define ENTITY_ATTRIBUTE_RESOLVER_H

#include "EntityAttribute.h"
#include <string>

namespace Project::Entities {
  class EntityAttributeResolver {
  public:
    static EntityAttribute resolve(const std::string& name);
  };
}

#endif
