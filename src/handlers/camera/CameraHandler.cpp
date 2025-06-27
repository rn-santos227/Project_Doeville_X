#include "CameraHandler.h"

namespace Project::Handlers {
  void CameraHandler::setSize(int width, int height) {
    cameraRect.w = width;
    cameraRect.h = height;
  }

  void CameraHandler::setPosition(int x, int y) {
    cameraRect.x = x;
    cameraRect.y = y;
  }
}
