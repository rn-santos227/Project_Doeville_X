#ifndef KEY_ACTION_RESOLVER_H
#define KEY_ACTION_RESOLVER_H

#include "KeyAction.h"
#include <string_view>

namespace Project::Handlers {
  class KeyActionResolver {
  public:
    static KeyAction resolve(std::string_view name);
  };
}

#endif
