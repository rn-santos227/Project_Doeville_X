#include "SurfaceTypeResolver.h"

#include <array>
#include <string_view>

#include "libraries/categories/SurfaceCategories.h"
#include "utilities/string/StringUtils.h"

namespace Project::Components {
  namespace Surfaces = Project::Libraries::Categories::Surfaces;

  SurfaceType SurfaceTypeResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, SurfaceType>, Surfaces::SURFACE_NAME_ALIAS_COUNT> map{{
      {Surfaces::BOUNCE, SurfaceType::BOUNCE},
      {Surfaces::DESTROY_ON_HIT, SurfaceType::DESTROY_ON_HIT},
      {Surfaces::GHOST_PASS, SurfaceType::GHOST_PASS},
      {Surfaces::REST, SurfaceType::REST},
      {Surfaces::SLIDE, SurfaceType::SLIDE},
      {Surfaces::STICK, SurfaceType::STICK},
      {Surfaces::TRIGGER_EVENT, SurfaceType::TRIGGER_EVENT},
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return SurfaceType::REST;
  }
}
