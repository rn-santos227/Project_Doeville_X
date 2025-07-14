#ifndef CAMERA_HANDLER_H
#define CAMERA_HANDLER_H

#include "CameraType.h"

#include <SDL2/SDL.h>

namespace Project::Handlers {
  class CameraHandler {
  public:
    CameraHandler() = default;
    ~CameraHandler() = default;

    void setSize(int width, int height);
    void setPosition(int x, int y);
    void setCullingOffset(int x, int y);
    SDL_Rect getCullingRect() const;

    void setCameraType(CameraType type);
    CameraType getCameraType() const { return cameraType; }

    int getWidth() const { return cameraRect.w; }
    int getHeight() const { return cameraRect.h; }
    int getX() const { return cameraRect.x; }
    int getY() const { return cameraRect.y; }
    
    const SDL_Rect& getRect() const { return cameraRect; }
    int getOffsetX() const { return cullingOffset.x; }
    int getOffsetY() const { return cullingOffset.y; }

  private:
    CameraType cameraType = CameraType::FREE_ROAMING;
    
    SDL_Rect cameraRect{0,0,0,0};
    SDL_Point cullingOffset{0,0};
  };
}

#endif
