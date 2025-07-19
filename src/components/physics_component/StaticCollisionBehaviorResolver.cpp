#include "StaticCollisionBehaviorResolver.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>

#include "libraries/categories/Categories.h"

namespace Project::Components::Physics {
  namespace Surfaces = Project::Libraries::Categories::Surfaces;
  StaticCollisionBehavior StaticCollisionBehaviorResolver::resolve(const std::string& name) {
    static const std::unordered_map<std::string, StaticCollisionBehavior> map = {
      {std::string(Surfaces::BOUNCE), StaticCollisionBehavior::BOUNCE},
      {std::string(Surfaces::DESTROY_ON_HIT), StaticCollisionBehavior::DESTROY_ON_HIT},
      {std::string(Surfaces::GHOST_PASS), StaticCollisionBehavior::GHOST_PASS},
      {std::string(Surfaces::REST), StaticCollisionBehavior::REST},
      {std::string(Surfaces::SLIDE), StaticCollisionBehavior::SLIDE},
      {std::string(Surfaces::STICK), StaticCollisionBehavior::STICK},
      {std::string(Surfaces::TRIGGER_EVENT), StaticCollisionBehavior::TRIGGER_EVENT},
    };

    std::string key = name;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
      return std::toupper(c);
    });

    auto it = map.find(key);
    if (it != map.end()) return it->second;
    return StaticCollisionBehavior::REST;
  }
}
