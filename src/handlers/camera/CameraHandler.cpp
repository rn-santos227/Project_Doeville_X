#include "CameraHandler.h"

#include <algorithm>

#include "libraries/constants/FloatConstants.h"
#include "libraries/constants/IndexConstants.h"

namespace Project::Handlers {
  void CameraHandler::setSize(int width, int height) {
    cameraRect.w = static_cast<float>(width);
    cameraRect.h = static_cast<float>(height);
  }

  void CameraHandler::setPosition(float x, float y) {
    cameraRect.x = x;
    cameraRect.y = y;
  }

  void CameraHandler::setCullingOffset(int x, int y) {
    cullingOffset.x = x;
    cullingOffset.y = y;
  }

  SDL_FRect CameraHandler::getCullingRect() const {
    SDL_FRect rect{cameraRect.x, cameraRect.y, cameraRect.w, cameraRect.h};
    rect.w /= zoom;
    rect.h /= zoom;
    rect.x -= static_cast<float>(cullingOffset.x);
    rect.y -= static_cast<float>(cullingOffset.y);
    rect.w += static_cast<float>(cullingOffset.x * Project::Libraries::Constants::INDEX_TWO);
    rect.h += static_cast<float>(cullingOffset.y * Project::Libraries::Constants::INDEX_TWO);
    return rect;
  }

  SDL_FRect CameraHandler::getRect() const {
    SDL_FRect rect{cameraRect.x, cameraRect.y, cameraRect.w, cameraRect.h};
    rect.w /= zoom;
    rect.h /= zoom;
    return rect;
  }

  void CameraHandler::setCameraType(CameraType type) {
    cameraType = type;
  }

  void CameraHandler::setZoom(float factor) {
    zoom = std::max(Project::Libraries::Constants::DEFAULT_CAMERA_ZOOM_STEP, factor);
  }

  void CameraHandler::zoomIn(float amount) {
    setZoom(zoom + amount);
  }

  void CameraHandler::zoomOut(float amount) {
    setZoom(zoom - amount);
  }

  void CameraHandler::setRotation(float angle) {
    rotation = angle;
  }

  void CameraHandler::rotate(float amount) {
    rotation += amount;
  }
}
