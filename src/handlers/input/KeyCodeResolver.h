#ifndef KEY_CODE_RESOLVER_H
#define KEY_CODE_RESOLVER_H

#include <SDL2/SDL.h>
#include <string>

namespace Project::Handlers {
  class KeyCodeResolver {
  public:
    static SDL_Scancode resolve(const std::string& name);
  };
}

#endif
