#ifndef CAMERA_HANDLER_H
#define CAMERA_HANDLER_H

#include <SDL2/SDL.h>

namespace Project::Handlers {
  class CameraHandler {
  public:
    CameraHandler() = default;
    ~CameraHandler() = default;

    void setSize(int width, int height);
  };
}
#endif
