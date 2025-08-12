#ifndef CHUNK_SIZE_H
#define CHUNK_SIZE_H

namespace Project::Entities {
  struct ChunkSize {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    ChunkSize() = default;
    ChunkSize(float xVal, float yVal, float zVal = 0.0f)
      : x(xVal), y(yVal), z(zVal) {}
  };
}

#endif
