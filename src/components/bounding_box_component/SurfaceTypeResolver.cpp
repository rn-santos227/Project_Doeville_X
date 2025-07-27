#include "SurfaceTypeResolver.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>

#include "libraries/categories/Categories.h"

namespace Project::Components {
  namespace Surfaces = Project::Libraries::Categories::Surfaces;
  SurfaceType SurfaceTypeResolver::resolve(const std::string& name) {
    static const std::unordered_map<std::string, SurfaceType> map = {
      {std::string(Surfaces::BOUNCE), SurfaceType::BOUNCE},
      {std::string(Surfaces::DESTROY_ON_HIT), SurfaceType::DESTROY_ON_HIT},
      {std::string(Surfaces::GHOST_PASS), SurfaceType::GHOST_PASS},
      {std::string(Surfaces::REST), SurfaceType::REST},
      {std::string(Surfaces::SLIDE), SurfaceType::SLIDE},
      {std::string(Surfaces::STICK), SurfaceType::STICK},
      {std::string(Surfaces::TRIGGER_EVENT), SurfaceType::TRIGGER_EVENT},
    };

    std::string key = name;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
      return std::toupper(c);
    });

    auto it = map.find(key);
    if (it != map.end()) return it->second;
    return SurfaceType::REST;
  }
}
