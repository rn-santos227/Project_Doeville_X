#ifndef CURSOR_STATE_H
#define CURSOR_STATE_H

namespace Project::Handlers {
  enum class CursorState {
    DEFAULT,
    CUSTOM,
    CROSSHAIR,
    HOVER,
    CLICK,
    ATTACK,
    DISABLED
  };
}

#endif