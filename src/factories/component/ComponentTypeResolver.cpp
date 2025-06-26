#include "ComponentTypeResolver.h"

#include <algorithm>
#include <cctype>

ComponentType ComponentTypeResolver::resolve(const std::string& name) {
  static const std::unordered_map<std::string, ComponentType> map = {
    {"GraphicsComponent", ComponentType::GRAPHICS},
    {"TextComponent", ComponentType::TEXT},
  };

  auto it = map.find(name);
  if (it != map.end()) return it->second;
  return ComponentType::UNKNOWN;
}
