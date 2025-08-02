#ifndef CAMERA_DATA_H
#define CAMERA_DATA_H

#include <string>

#include "libraries/constants/FloatConstants.h"

namespace Project::Components {
  struct CameraData {
    float zoom = Project::Libraries::Constants::DEFAULT_CAMERA_ZOOM;
    float followSpeed = Project::Libraries::Constants::DEFAULT_CAMERA_SPEED;
    float rotation = Project::Libraries::Constants::DEFAULT_CAMERA_ROTATION;
    float spinSpeed = Project::Libraries::Constants::DEFAULT_CAMERA_SPIN_SPEED;

    float offsetX = 0.0f;
    float offsetY = 0.0f;

    std::string targetName;
  };
}

#endif
