#ifndef FLOAT_CONSTANTS_H
#define FLOAT_CONSTANTS_H

namespace Project::Libraries::Constants {
  constexpr float DEFAULT_CELL_SIZE = 128.0f;
  constexpr float DEFAULT_CHUNK_SIZE = 512.0f;
  constexpr float DEFAULT_SPAWN_RADIUS = 300.0f;

  constexpr float DEFAULT_CAMERA_ZOOM = 1.0f;
  constexpr float DEFAULT_CAMERA_SPEED = 1.0f;
  constexpr float DEFAULT_CAMERA_ROTATION = 0.0f;
  constexpr float DEFAULT_CAMERA_SPIN_SPEED = 0.0f;

  constexpr float MIN_CELL = 32.0f;
  constexpr float MAX_CELL = 256.0f;
}

#endif
