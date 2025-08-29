#ifndef ERROR_CATEGORY_H
#define ERROR_CATEGORY_H

namespace Project::Utilities {
  enum class ErrorCategory {
    SDL,
    CONFIG,
    RESOURCE,
    GAME_STATE,
    SCRIPTING,
    RUNTIME,
    INPUT,
    UNKNOWN
  };
}

#endif
