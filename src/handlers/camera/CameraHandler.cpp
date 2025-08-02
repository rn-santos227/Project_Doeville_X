#include "CameraHandler.h"

#include <algorithm>

#include "libraries/constants/IndexConstants.h"

namespace Project::Handlers {
  void CameraHandler::setSize(int width, int height) {
    cameraRect.w = width;
    cameraRect.h = height;
  }

  void CameraHandler::setPosition(int x, int y) {
    cameraRect.x = x;
    cameraRect.y = y;
  }

  void CameraHandler::setCullingOffset(int x, int y) {
    cullingOffset.x = x;
    cullingOffset.y = y;
  }

  SDL_Rect CameraHandler::getCullingRect() const {
    SDL_Rect rect = cameraRect;
    rect.w = static_cast<int>(cameraRect.w / zoom);
    rect.h = static_cast<int>(cameraRect.h / zoom);
    rect.x -= cullingOffset.x;
    rect.y -= cullingOffset.y;
    rect.w += cullingOffset.x * Project::Libraries::Constants::INDEX_TWO;
    rect.h += cullingOffset.y * Project::Libraries::Constants::INDEX_TWO;
    return rect;
  }

  void CameraHandler::setCameraType(CameraType type) {
    cameraType = type;
  }

  void CameraHandler::setZoom(float factor) {
    if (factor > 0.0f) {
      zoom = factor;
    }
  }

  void CameraHandler::zoomIn(float amount) {
    setZoom(zoom + amount);
  }

  void CameraHandler::zoomOut(float amount) {
    setZoom(std::max(0.01f, zoom - amount));
  }

  void CameraHandler::setRotation(float angle) {
    rotation = angle;
  }

  void CameraHandler::rotate(float amount) {
    rotation += amount;
  }

  SDL_Rect CameraHandler::getRect() const {
    SDL_Rect rect = cameraRect;
    rect.w = static_cast<int>(cameraRect.w / zoom);
    rect.h = static_cast<int>(cameraRect.h / zoom);
    return rect;
  }
}
