#ifndef CAMERA_HANDLER_H
#define CAMERA_HANDLER_H

#include <SDL2/SDL.h>

namespace Project::Handlers {
  class CameraHandler {
  public:
    CameraHandler() = default;
    ~CameraHandler() = default;

    void setSize(int width, int height);
    void setPosition(int x, int y);

    int getWidth() const { return cameraRect.w; }

    private:
      SDL_Rect cameraRect{0,0,0,0};
  };
}
#endif
