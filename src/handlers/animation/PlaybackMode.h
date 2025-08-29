#ifndef PLAYBACK_MODE_H
#define PLAYBACK_MODE_H

namespace Project::Handlers {
  enum class PlaybackMode {
    NONE,
    LOOP,
    ONCE,
    PINGPONG,
    REVERSE,
    CUSTOM
  };
}

#endif
