#include "CameraTypeResolver.h"

#include <array>
#include <string_view>

#include "libraries/categories/Categories.h"
#include "libraries/constants/NumericConstants.h"
#include "utilities/string/StringUtils.h"

namespace Project::Handlers {
  namespace Cameras = Project::Libraries::Categories::Cameras;
  namespace Constants = Project::Libraries::Constants;

  CameraType CameraTypeResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, CameraType>, Constants::CAMERA_NAME_ALIAS_COUNT> map{{
      {Cameras::TOP_DOWN, CameraType::TOP_DOWN},
      {Cameras::SIDE_SCROLLER, CameraType::SIDE_SCROLLER},
      {Cameras::ISOMETRIC, CameraType::ISOMETRIC},
      {Cameras::PLATFORMER, CameraType::PLATFORMER},
      {Cameras::BIRDS_EYE, CameraType::BIRDS_EYE},
      {Cameras::FIRST_PERSON, CameraType::FIRST_PERSON},
      {Cameras::THIRD_PERSON, CameraType::THIRD_PERSON},
      {Cameras::OVER_SHOULDER, CameraType::OVER_SHOULDER},
      {Cameras::FIXED, CameraType::FIXED},
      {Cameras::TWO_POINT_FIVE_D, CameraType::TWO_POINT_FIVE_D},
      {Cameras::FREE_ROAMING, CameraType::FREE_ROAMING},
      {Cameras::VR, CameraType::VR}
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return CameraType::FREE_ROAMING;
  };
}
