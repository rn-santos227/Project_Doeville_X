#include "CameraTypeResolver.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>

#include "libraries/categories/Categories.h"

namespace Project::Handlers {
  namespace Cameras = Project::Libraries::Categories::Cameras;
  CameraType CameraTypeResolver::resolve(const std::string& name) {
    static const std::unordered_map<std::string, CameraType> map = {
      {std::string(Cameras::TOP_DOWN), CameraType::TOP_DOWN},
      {std::string(Cameras::SIDE_SCROLLER), CameraType::SIDE_SCROLLER},
      {std::string(Cameras::ISOMETRIC), CameraType::ISOMETRIC},
      {std::string(Cameras::PLATFORMER), CameraType::PLATFORMER},
      {std::string(Cameras::BIRDS_EYE), CameraType::BIRDS_EYE},
      {std::string(Cameras::FIRST_PERSON), CameraType::FIRST_PERSON},
      {std::string(Cameras::THIRD_PERSON), CameraType::THIRD_PERSON},
      {std::string(Cameras::OVER_SHOULDER), CameraType::OVER_SHOULDER},
      {std::string(Cameras::FIXED), CameraType::FIXED},
      {std::string(Cameras::TWO_POINT_FIVE_D), CameraType::TWO_POINT_FIVE_D},
      {std::string(Cameras::FREE_ROAMING), CameraType::FREE_ROAMING},
      {std::string(Cameras::VR), CameraType::VR}
    };
  };
}
