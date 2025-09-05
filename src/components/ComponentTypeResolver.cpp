#include "ComponentTypeResolver.h"

#include <algorithm>
#include <array>
#include <string_view>

#include "libraries/categories/Categories.h"
#include "utilities/string/StringUtils.h"

namespace Project::Components {
  namespace Components = Project::Libraries::Categories::Components;

  ComponentType ComponentTypeResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, ComponentType>, Components::COMPONENT_NAME_ALIAS_COUNT> map{{
      {Components::BEHAVIOR, ComponentType::BEHAVIOR},
      {Components::BEHAVIOR_COMPONENT, ComponentType::BEHAVIOR},
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
      {Components::LIGHT, ComponentType::LIGHT},
      {Components::LIGHT_COMPONENT, ComponentType::LIGHT},
      {Components::METER, ComponentType::METER},
      {Components::METER_COMPONENT, ComponentType::METER},
      {Components::MODAL, ComponentType::MODAL},
      {Components::MODAL_COMPONENT, ComponentType::MODAL},
      {Components::MOTION, ComponentType::MOTION},
      {Components::MOTION_COMPONENT, ComponentType::MOTION},
      {Components::NETWORK, ComponentType::NETWORK},
      {Components::NETWORK_COMPONENT, ComponentType::NETWORK},
      {Components::NUMERIC, ComponentType::NUMERIC},
      {Components::NUMERIC_COMPONENT, ComponentType::NUMERIC},
      {Components::PHYSICS, ComponentType::PHYSICS},
      {Components::PHYSICS_COMPONENT, ComponentType::PHYSICS},
      {Components::PORTAL, ComponentType::PORTAL},
      {Components::PORTAL_COMPONENT, ComponentType::PORTAL},
      {Components::SPAWNER, ComponentType::SPAWNER},
      {Components::SPAWNER_COMPONENT, ComponentType::SPAWNER},
      {Components::TEXT, ComponentType::TEXT},
      {Components::TEXT_COMPONENT, ComponentType::TEXT},
      {Components::TIMER, ComponentType::TIMER},
      {Components::TIMER_COMPONENT, ComponentType::TIMER},
      {Components::TRANSFORM, ComponentType::TRANSFORM},
      {Components::TRANSFORM_COMPONENT, ComponentType::TRANSFORM},
      {Components::VISION, ComponentType::VISION},
      {Components::VISION_COMPONENT, ComponentType::VISION}
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return ComponentType::UNKNOWN;
  }
}
