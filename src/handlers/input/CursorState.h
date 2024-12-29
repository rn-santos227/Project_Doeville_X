#ifndef CURSOR_STATE_H
#define CURSOR_STATE_H

namespace Project::Handlers {
  enum class CursorState {
    DEFAULT,
    CUSTOM,
    HOVER,
    CLICK,
    ATTACK,
    DISABLED
  };
}

#endif