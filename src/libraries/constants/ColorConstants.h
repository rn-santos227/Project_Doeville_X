#ifndef COLOR_CONSTANTS_H
#define COLOR_CONSTANTS_H

#include <SDL.h>

namespace Project::Libraries::Constants {
  constexpr SDL_Color DEFAULT_DEBUG_TEXT_COLOR{144, 238, 144, 255};
  constexpr SDL_Color DEFAULT_BOUNDING_BOX_COLOR{0, 0, 0, 255};
  constexpr SDL_Color DEFAULT_SHAPE_COLOR{255, 255, 255, 255};
  constexpr SDL_Color DEFAULT_BACKGROUND_COLOR{0, 0, 0, 255};
  constexpr SDL_Color DEFAULT_X_AXIS_COLOR{255, 0, 0, 255};
  constexpr SDL_Color DEFAULT_Y_AXIS_COLOR{0, 0, 255, 255};
  constexpr SDL_Color DEFAULT_Z_AXIS_COLOR{0, 255, 0, 255};
  constexpr SDL_Color DEFAULT_GRID_COLOR{0, 100, 0, 255};
  constexpr SDL_Color DEFAULT_LIGHT_COLOR{255, 255, 200, 255};

  constexpr const char* DEFAULT_DEBUG_TEXT_COLOR_HEX = "#90EE90";
  constexpr const char* DEFAULT_BOUNDING_BOX_COLOR_HEX = "#000000";
  constexpr const char* DEFAULT_SHAPE_COLOR_HEX = "#FFFFFF";
  constexpr const char* DEFAULT_LIGHT_COLOR_HEX = "#FFFFC8";

  constexpr SDL_Color COLOR_RED{255, 0, 0, 255};
  constexpr SDL_Color COLOR_GREEN{0, 255, 0, 255};
  constexpr SDL_Color COLOR_BLUE{0, 0, 255, 255};

  constexpr SDL_Color COLOR_YELLOW{255, 255, 0, 255};
  constexpr SDL_Color COLOR_CYAN{0, 255, 255, 255};
  constexpr SDL_Color COLOR_MAGENTA{255, 0, 255, 255};

  constexpr SDL_Color COLOR_BLACK{0, 0, 0, 255};
  constexpr SDL_Color COLOR_WHITE{255, 255, 255, 255};
  constexpr SDL_Color COLOR_GRAY{128, 128, 128, 255};

  constexpr const char* COLOR_HEX_RED = "#FF0000";
  constexpr const char* COLOR_HEX_GREEN = "#00FF00";
  constexpr const char* COLOR_HEX_BLUE = "#0000FF";
  constexpr const char* COLOR_HEX_YELLOW = "#FFFF00";
  constexpr const char* COLOR_HEX_CYAN = "#00FFFF";
  constexpr const char* COLOR_HEX_MAGENTA = "#FF00FF";
  constexpr const char* COLOR_HEX_BLACK = "#000000";
  constexpr const char* COLOR_HEX_WHITE = "#FFFFFF";
  constexpr const char* COLOR_HEX_GRAY = "#808080";
}

#endif
