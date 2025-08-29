#ifndef CAMERA_TYPE_RESOLVER_H
#define CAMERA_TYPE_RESOLVER_H

#include "CameraType.h"
#include <string_view>

namespace Project::Handlers {
  class CameraTypeResolver {
  public:
    static CameraType resolve(std::string_view name);
  };
}

#endif
