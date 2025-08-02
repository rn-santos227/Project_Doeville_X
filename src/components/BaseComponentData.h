#ifndef BASE_COMPONENT_DATA_H
#define BASE_COMPONENT_DATA_H

#include <string>

#include "ComponentType.h"

namespace Project::Components {
  struct BaseComponentData {
    std::string componentClass;
    std::string componentID;
    ComponentType componentType;
    bool active = true;
  };
}

#endif
