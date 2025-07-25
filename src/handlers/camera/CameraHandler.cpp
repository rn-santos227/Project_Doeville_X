#include "CameraHandler.h"

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
    rect.x -= cullingOffset.x;
    rect.y -= cullingOffset.y;
    rect.w += cullingOffset.x * Project::Libraries::Constants::INDEX_TWO;
    rect.h += cullingOffset.y * Project::Libraries::Constants::INDEX_TWO;
    return rect;
  }

  void CameraHandler::setCameraType(CameraType type) {
    cameraType = type;
  }
}
