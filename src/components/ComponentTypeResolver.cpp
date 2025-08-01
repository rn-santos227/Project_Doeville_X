#include "ComponentTypeResolver.h"

#include <algorithm>
#include <array>
#include <string_view>

#include "libraries/categories/Categories.h"
#include "utilities/string/StringUtils.h"

namespace Project::Components {
  namespace Components = Project::Libraries::Categories::Components;
  ComponentType ComponentTypeResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, ComponentType>, 28> map{{
      {Components::BOUNDING_BOX, ComponentType::BOUNDING_BOX},
      {Components::BOUNDING_BOX_COMPONENT, ComponentType::BOUNDING_BOX},
      {Components::BUTTON, ComponentType::BUTTON},
      {Components::BUTTON_COMPONENT, ComponentType::BUTTON},
      {Components::CAMERA, ComponentType::CAMERA},
      {Components::CAMERA_COMPONENT, ComponentType::CAMERA},
      {Components::GRAPHICS, ComponentType::GRAPHICS},
      {Components::GRAPHICS_COMPONENT, ComponentType::GRAPHICS},
      {Components::INPUT, ComponentType::INPUT},
      {Components::INPUT_COMPONENT, ComponentType::INPUT},
      {Components::KEYS, ComponentType::KEYS},
      {Components::KEYS_COMPONENT, ComponentType::KEYS},
      {Components::MODAL, ComponentType::MODAL},
      {Components::MODAL_COMPONENT, ComponentType::MODAL},
      {Components::MOTION, ComponentType::MOTION},
      {Components::MOTION_COMPONENT, ComponentType::MOTION},
      {Components::NUMERIC, ComponentType::NUMERIC},
      {Components::NUMERIC_COMPONENT, ComponentType::NUMERIC},
      {Components::PHYSICS, ComponentType::PHYSICS},
      {Components::PHYSICS_COMPONENT, ComponentType::PHYSICS},
      {Components::SPAWNER, ComponentType::SPAWNER},
      {Components::SPAWNER_COMPONENT, ComponentType::SPAWNER},
      {Components::TEXT, ComponentType::TEXT},
      {Components::TEXT_COMPONENT, ComponentType::TEXT},
      {Components::TIMER, ComponentType::TIMER},
      {Components::TIMER_COMPONENT, ComponentType::TIMER},
      {Components::TRANSFORM, ComponentType::TRANSFORM},
      {Components::TRANSFORM_COMPONENT, ComponentType::TRANSFORM}
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return ComponentType::UNKNOWN;
  }
}
