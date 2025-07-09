#include "KeyCodeResolver.h"
#include <algorithm>

namespace Project::Handlers {
  SDL_Scancode KeyCodeResolver::resolve(const std::string& name) {
    if (name.empty()) return SDL_SCANCODE_UNKNOWN;
  }
}
