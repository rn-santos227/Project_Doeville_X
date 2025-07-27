#include "EntityAttributeResolver.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>

#include "libraries/attributes/Attributes.h"

namespace Project::Entities {
  namespace Attributes = Project::Libraries::Attributes::Entities;
  EntityAttribute EntityAttributeResolver::resolve(const std::string& name) {
    static const std::unordered_map<std::string, EntityAttribute> map = {
      {std::string(Attributes::DEFAULT), EntityAttribute::DEFAULT},
      {std::string(Attributes::AGGRESSIVE), EntityAttribute::AGGRESSIVE},
      {std::string(Attributes::DEBUG_ONLY), EntityAttribute::DEBUG_ONLY},
      {std::string(Attributes::DEFERRED_INIT), EntityAttribute::DEFERRED_INIT},
      {std::string(Attributes::FLOATING), EntityAttribute::FLOATING},
      {std::string(Attributes::GHOST), EntityAttribute::GHOST},
      {std::string(Attributes::HIGH_PRIORITY), EntityAttribute::HIGH_PRIORITY},
      {std::string(Attributes::INDEPENDENT), EntityAttribute::INDEPENDENT},
      {std::string(Attributes::INTERACTIVE), EntityAttribute::INTERACTIVE},
      {std::string(Attributes::LAZY_UPDATE), EntityAttribute::LAZY_UPDATE},
      {std::string(Attributes::LOW_PRIORITY), EntityAttribute::LOW_PRIORITY},
      {std::string(Attributes::NO_ANIMATION), EntityAttribute::NO_ANIMATION},
      {std::string(Attributes::NO_CLIP), EntityAttribute::NO_CLIP},
      {std::string(Attributes::PASSIVE), EntityAttribute::PASSIVE},
      {std::string(Attributes::PAUSED_WHEN_HIDDEN), EntityAttribute::PAUSED_WHEN_HIDDEN},
      {std::string(Attributes::PERMANENT), EntityAttribute::PERMANENT},
      {std::string(Attributes::PERSISTENT), EntityAttribute::PERSISTENT},
      {std::string(Attributes::PERMANENT), EntityAttribute::PERMANENT},
      {std::string(Attributes::RESTING), EntityAttribute::RESTING},
      {std::string(Attributes::RESOURCE_LIGHT), EntityAttribute::RESOURCE_LIGHT},
      {std::string(Attributes::SCRIPTED), EntityAttribute::SCRIPTED},
      {std::string(Attributes::SLEEPING), EntityAttribute::SLEEPING},
      {std::string(Attributes::VOLATILE), EntityAttribute::VOLATILE},
    };

    std::string key = name;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
      return std::toupper(c);
    });

    auto it = map.find(key);
    if (it != map.end()) return it->second;
    return EntityAttribute::DEFAULT;
  }
}
