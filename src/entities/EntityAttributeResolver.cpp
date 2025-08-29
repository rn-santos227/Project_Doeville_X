#include "EntityAttributeResolver.h"

#include <array>
#include <string_view>

#include "libraries/attributes/Attributes.h"
#include "utilities/string/StringUtils.h"

namespace Project::Entities {
  namespace Attributes = Project::Libraries::Attributes::Entities;
  EntityAttribute EntityAttributeResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, EntityAttribute>, 25> map{{
      {Attributes::DEFAULT, EntityAttribute::DEFAULT},
      {Attributes::AGGRESSIVE, EntityAttribute::AGGRESSIVE},
      {Attributes::DEBUG_ONLY, EntityAttribute::DEBUG_ONLY},
      {Attributes::DEFERRED_INIT, EntityAttribute::DEFERRED_INIT},
      {Attributes::DISPOSABLE, EntityAttribute::DISPOSABLE},
      {Attributes::FLOATING, EntityAttribute::FLOATING},
      {Attributes::GHOST, EntityAttribute::GHOST},
      {Attributes::IMMOVABLE, EntityAttribute::IMMOVABLE},
      {Attributes::HIGH_PRIORITY, EntityAttribute::HIGH_PRIORITY},
      {Attributes::INDEPENDENT, EntityAttribute::INDEPENDENT},
      {Attributes::INTERACTIVE, EntityAttribute::INTERACTIVE},
      {Attributes::LAZY_UPDATE, EntityAttribute::LAZY_UPDATE},
      {Attributes::LOW_PRIORITY, EntityAttribute::LOW_PRIORITY},
      {Attributes::NO_ANIMATION, EntityAttribute::NO_ANIMATION},
      {Attributes::NO_CLIP, EntityAttribute::NO_CLIP},
      {Attributes::PASSIVE, EntityAttribute::PASSIVE},
      {Attributes::PAUSED_WHEN_HIDDEN, EntityAttribute::PAUSED_WHEN_HIDDEN},
      {Attributes::PERMANENT, EntityAttribute::PERMANENT},
      {Attributes::PERSISTENT, EntityAttribute::PERSISTENT},
      {Attributes::RESTING, EntityAttribute::RESTING},
      {Attributes::RESOURCE_LIGHT, EntityAttribute::RESOURCE_LIGHT},
      {Attributes::SCRIPTED, EntityAttribute::SCRIPTED},
      {Attributes::SLEEPING, EntityAttribute::SLEEPING},
      {Attributes::UNBOUNDED, EntityAttribute::UNBOUNDED},
      {Attributes::VOLATILE, EntityAttribute::VOLATILE},
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return EntityAttribute::DEFAULT;
  }
}
