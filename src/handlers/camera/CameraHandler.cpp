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

  SDL_FRect CameraHandler::getCullingRect() const {
    SDL_FRect rect{
      static_cast<float>(cameraRect.x),
      static_cast<float>(cameraRect.y),
      static_cast<float>(cameraRect.w),
      static_cast<float>(cameraRect.h)
    };
    rect.w /= zoom;
    rect.h /= zoom;
    rect.x -= static_cast<float>(cullingOffset.x);
    rect.y -= static_cast<float>(cullingOffset.y);
    rect.w += static_cast<float>(cullingOffset.x * Project::Libraries::Constants::INDEX_TWO);
    rect.h += static_cast<float>(cullingOffset.y * Project::Libraries::Constants::INDEX_TWO);
    return rect;
  }

  SDL_FRect CameraHandler::getRect() const {
    SDL_FRect rect{
      static_cast<float>(cameraRect.x),
      static_cast<float>(cameraRect.y),
      static_cast<float>(cameraRect.w),
      static_cast<float>(cameraRect.h)
    };
    rect.w /= zoom;
    rect.h /= zoom;
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
}
