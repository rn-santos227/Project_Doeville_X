#ifndef KEY_ACTION_RESOLVER_H
#define KEY_ACTION_RESOLVER_H

#include "KeyAction.h"
#include <string>
#include <unordered_map>

namespace Project::Handlers {
  class KeyActionResolver {
  public:
    static KeyAction resolve(const std::string& name);
  };
}

#endif
