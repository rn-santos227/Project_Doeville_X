#include "ComponentTypeResolver.h"

ComponentType ComponentTypeResolver::resolve(const std::string& name) {
  static const std::unordered_map<std::string, ComponentType> map = {
    {"GraphicsComponent", ComponentType::GRAPHICS},
    {"TextComponent", ComponentType::TEXT},
  };
}