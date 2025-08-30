#ifndef FLOAT_CONSTANTS_H
#define FLOAT_CONSTANTS_H

namespace Project::Libraries::Constants {
  constexpr float DEFAULT_CELL_SIZE = 128.0f;
  constexpr float DEFAULT_CHUNK_SIZE = 512.0f;
  constexpr float DEFAULT_SPAWN_RADIUS = 300.0f;

  constexpr float DEFAULT_CAMERA_ZOOM = 1.0f;
  constexpr float DEFAULT_CAMERA_ZOOM_SPEED = 0.01f;
  constexpr float DEFAULT_CAMERA_SPEED = 1.0f;
  constexpr float DEFAULT_CAMERA_ROTATION = 0.0f;
  constexpr float DEFAULT_CAMERA_SPIN_SPEED = 0.0f;
  constexpr float DEFAULT_CAMERA_SHAKE_INTENSITY = 0.0f;
  constexpr float DEFAULT_CAMERA_ROTATION_STEP = 0.1f;
  constexpr float DEFAULT_CAMERA_SHAKE_DURATION = 1.5f;
  constexpr float DEFAULT_CAMERA_MIN_ZOOM = 0.01f;
  constexpr float DEFAULT_CAMERA_MAX_ZOOM = 10.0f;

  constexpr float DEFAULT_BAR_SPEED = 0.1f;

  constexpr float MIN_CELL = 32.0f;
  constexpr float MAX_CELL = 256.0f;

  constexpr float FLOAT_255 = 255.0f;

  constexpr float DEFAULT_DENOMINATOR = 2.0f;
  constexpr float DEFAULT_WHOLE = 1.0f;
  constexpr float DEFAULT_HALF = 0.5f;
  constexpr float DEFAULT_DOUBLE = 2.0;
  constexpr float DEFAULT_TRIPLE = 3.0;
  constexpr float FLOAT_HUNDRED = 100.0;

  constexpr float DEFAULT_LIGHT_RADIUS = 100.0f;
  constexpr float RAYCAST_MAX_DISTANCE = 1e9f;
  constexpr float RAYCAST_EPSILON = 1e-6f;

  constexpr float DEFAULT_FRECT_X = -1e9f;
  constexpr float DEFAULT_FRECT_Y = -1e9f;
  constexpr float DEFAULT_FRECT_W = 2e9;
  constexpr float DEFAULT_FRECT_H = 2e9f;
}

#endif
