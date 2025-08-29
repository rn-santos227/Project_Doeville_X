#ifndef MOVEMENT_MODE_RESOLVER_H
#define MOVEMENT_MODE_RESOLVER_H

#include "MovementMode.h"
#include <string_view>

namespace Project::Components {
  class MovementModeResolver {
  public:
    static MovementMode resolve(std::string_view name);
  };
}

#endif
