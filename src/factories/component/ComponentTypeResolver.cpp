#include "ComponentTypeResolver.h"

#include <algorithm>
#include <cctype>

#include "libraries/categories/Categories.h"

namespace Project::Factories {
  using Project::Factories::ComponentType;
  namespace Components = Project::Libraries::Categories::Components;

  ComponentType ComponentTypeResolver::resolve(const std::string& name) {
    static const std::unordered_map<std::string, ComponentType> map = {
      {std::string(Components::CAMERA), ComponentType::CAMERA},
      {std::string(Components::CAMERA) + Components::SUFFIX, ComponentType::CAMERA},
      {std::string(Components::BOUNDING_BOX), ComponentType::BOUNDING_BOX},
      {std::string(Components::BOUNDING_BOX) + Components::SUFFIX, ComponentType::BOUNDING_BOX},
      {std::string(Components::GRAPHICS), ComponentType::GRAPHICS},
      {std::string(Components::GRAPHICS) + Components::SUFFIX, ComponentType::GRAPHICS},
      {std::string(Components::KEYS), ComponentType::KEYS},
      {std::string(Components::KEYS) + Components::SUFFIX, ComponentType::KEYS},
      {std::string(Components::MOTION), ComponentType::MOTION},
      {std::string(Components::MOTION) + Components::SUFFIX, ComponentType::MOTION},
      {std::string(Components::PHYSICS), ComponentType::PHYSICS},
      {std::string(Components::PHYSICS) + Components::SUFFIX, ComponentType::PHYSICS},
      {std::string(Components::TEXT), ComponentType::TEXT},
      {std::string(Components::TEXT) + Components::SUFFIX, ComponentType::TEXT},
      {std::string(Components::TRANSFORM), ComponentType::TRANSFORM},
      {std::string(Components::TRANSFORM) + Components::SUFFIX, ComponentType::TRANSFORM}
    };

    std::string key = name;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return std::tolower(c); });

    auto it = map.find(key);
    if (it != map.end()) return it->second;
    return ComponentType::UNKNOWN;
  }
}
