#include "ComponentTypeResolver.h"

#include <algorithm>
#include <cctype>

namespace Project::Factories {
  using Project::Factories::ComponentType;
  ComponentType ComponentTypeResolver::resolve(const std::string& name) {
    static const std::unordered_map<std::string, ComponentType> map = {
      {"boundingboxcomponent", ComponentType::BOUNDING_BOX},
      {"boundingbox", ComponentType::BOUNDING_BOX},
      {"graphicscomponent", ComponentType::GRAPHICS},
      {"graphics", ComponentType::GRAPHICS},
      {"textcomponent", ComponentType::TEXT},
      {"text", ComponentType::TEXT},
    };

    std::string key = name;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return std::tolower(c); });

    auto it = map.find(key);
    if (it != map.end()) return it->second;
    return ComponentType::UNKNOWN;
  }
}
