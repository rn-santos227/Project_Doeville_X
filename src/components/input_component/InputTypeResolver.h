#ifndef INPUT_TYPE_RESOLVER_H
#define INPUT_TYPE_RESOLVER_H

#include "InputType.h"
#include <string_view>

namespace Project::Components {
  class InputTypeResolver {
  public:
    static InputType resolve(std::string_view name);
  };
}

#endif
