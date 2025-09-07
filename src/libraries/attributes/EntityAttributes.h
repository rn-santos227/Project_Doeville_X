#ifndef ENTITY_ATTRIBUTES_H
#define ENTITY_ATTRIBUTES_H

namespace Project::Libraries::Attributes {
  namespace Entities {
    constexpr size_t ENTITY_ATTRIBUTE_NAME_ALIAS_COUNT = 25;

    constexpr const char* DEFAULT = "DEFAULT";
    constexpr const char* AGGRESSIVE = "AGGRESSIVE";
    constexpr const char* DEBUG_ONLY = "DEBUG_ONLY";
    constexpr const char* DEFERRED_INIT = "DEFERRED_INIT";
    constexpr const char* DISPOSABLE = "DISPOSABLE";
    constexpr const char* FLOATING = "FLOATING";
    constexpr const char* GHOST = "GHOST";
    constexpr const char* HIGH_PRIORITY = "HIGH_PRIORITY";
    constexpr const char* IMMOVABLE = "IMMOVABLE";
    constexpr const char* INDEPENDENT = "INDEPENDENT";
    constexpr const char* INTERACTIVE = "INTERACTIVE";
    constexpr const char* LAZY_UPDATE = "LAZY_UPDATE";
    constexpr const char* LOW_PRIORITY = "LOW_PRIORITY";
    constexpr const char* NO_ANIMATION = "NO_ANIMATION";
    constexpr const char* NO_CLIP = "NO_CLIP";
    constexpr const char* PASSIVE = "PASSIVE";
    constexpr const char* PAUSED_WHEN_HIDDEN = "PAUSED_WHEN_HIDDEN";
    constexpr const char* PERMANENT = "PERMANENT";
    constexpr const char* PERSISTENT = "PERSISTENT";
    constexpr const char* RESTING = "RESTING";
    constexpr const char* RESOURCE_LIGHT = "RESOURCE_LIGHT";
    constexpr const char* SCRIPTED = "SCRIPTED";
    constexpr const char* SLEEPING = "SLEEPING";
    constexpr const char* UNBOUNDED = "UNBOUNDED";
    constexpr const char* VOLATILE  = "VOLATILE";
  }
}

#endif
