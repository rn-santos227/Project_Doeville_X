#ifndef CAMERA_TYPE_RESOLVER_H
#define CAMERA_TYPE_RESOLVER_H

#include "CameraType.h"
#include <string>

namespace Project::Handlers {
  class CameraTypeResolver {
  public:
    static CameraType resolve(const std::string& name);
  };
}

#endif
