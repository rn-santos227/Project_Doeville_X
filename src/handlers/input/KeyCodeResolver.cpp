#include "KeyCodeResolver.h"

#include <algorithm>
#include <string>

#include "libraries/constants/ScanCodeKeys.h"
#include "utilities/string/StringUtils.h"

namespace Project::Handlers {
  namespace ScanCodes = Project::Libraries::Constants;
  SDL_Scancode KeyCodeResolver::resolve(const std::string& name) {
    if (name.empty()) return SDL_SCANCODE_UNKNOWN;

    SDL_Scancode code = SDL_GetScancodeFromName(name.c_str());
    if (code >= SDL_SCANCODE_F1 && code <= SDL_SCANCODE_F12) {
      return SDL_SCANCODE_UNKNOWN;
    }

    if (code != SDL_SCANCODE_UNKNOWN) return code;

    if (Project::Utilities::StringUtils::iequals(name, ScanCodes::WORD_MINUS) || 
      name == std::string(1, ScanCodes::SYM_MINUS)) {
      return SDL_SCANCODE_MINUS;
    }
    if (Project::Utilities::StringUtils::iequals(name, ScanCodes::WORD_EQUALS) ||
        Project::Utilities::StringUtils::iequals(name, ScanCodes::WORD_PLUS) ||
        name == std::string(1, ScanCodes::SYM_EQUALS) ||
        name == std::string(1, ScanCodes::SYM_PLUS)) {
      return SDL_SCANCODE_EQUALS;
    }

    if (name.size() == 1) {
      char c = name[0];
      if (c >= 'a' && c <= 'z') {
        code = SDL_GetScancodeFromKey(SDLK_a + (c - 'a'));
      } else if (c >= 'A' && c <= 'Z') {
        code = SDL_GetScancodeFromKey(SDLK_a + (c - 'A'));
      }
    }

    if (code >= SDL_SCANCODE_F1 && code <= SDL_SCANCODE_F12) {
      return SDL_SCANCODE_UNKNOWN;
    }

    return code;
  }
}
