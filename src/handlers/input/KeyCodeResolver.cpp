#include "KeyCodeResolver.h"
#include <algorithm>

namespace Project::Handlers {
  SDL_Scancode KeyCodeResolver::resolve(const std::string& name) {
    if (name.empty()) return SDL_SCANCODE_UNKNOWN;

    SDL_Scancode code = SDL_GetScancodeFromName(name.c_str());
    if (code >= SDL_SCANCODE_F1 && code <= SDL_SCANCODE_F12) {
      return SDL_SCANCODE_UNKNOWN;
    }

    if (code != SDL_SCANCODE_UNKNOWN) return code;
  }
}
