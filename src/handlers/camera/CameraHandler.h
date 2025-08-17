#ifndef CAMERA_HANDLER_H
#define CAMERA_HANDLER_H

#include "CameraType.h"

#include <SDL2/SDL.h>

#include "libraries/constants/FloatConstants.h"

namespace Project::Handlers {
  class CameraHandler {
  public:
    CameraHandler() = default;
    ~CameraHandler() = default;

    void setSize(int width, int height);
    void setPosition(float x, float y);
    void setCullingOffset(int x, int y);
    
    SDL_FRect getCullingRect() const;
    SDL_FRect getRect() const;

    void setCameraType(CameraType type);
    CameraType getCameraType() const { return cameraType; }

    void setZoom(float factor);
    float getZoom() const { return zoom; }
    void zoomIn(float amount);
    void zoomOut(float amount);

    void setRotation(float angle);
    float getRotation() const { return rotation; }
    void rotate(float amount);

    int getWidth() const { return static_cast<int>(cameraRect.w / zoom); }
    int getHeight() const { return static_cast<int>(cameraRect.h / zoom); }
    float getX() const { return cameraRect.x; }
    float getY() const { return cameraRect.y; }
    int getViewportWidth() const { return static_cast<int>(cameraRect.w); }
    int getViewportHeight() const { return static_cast<int>(cameraRect.h); }

    int getOffsetX() const { return cullingOffset.x; }
    int getOffsetY() const { return cullingOffset.y; }

  private:
    CameraType cameraType = CameraType::FREE_ROAMING;
    
    SDL_FRect cameraRect{0.0f, 0.0f, 0.0f, 0.0f};
    SDL_Point cullingOffset{0,0};

    float zoom = Project::Libraries::Constants::DEFAULT_CAMERA_ZOOM;
    float rotation = Project::Libraries::Constants::DEFAULT_CAMERA_ROTATION;
  };
}

#endif
