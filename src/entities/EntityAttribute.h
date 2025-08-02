#ifndef ENTITY_ATTRIBUTE_H
#define ENTITY_ATTRIBUTE_H

namespace Project::Entities {
  enum class EntityAttribute {
    DEFAULT,
    AGGRESSIVE,
    DEBUG_ONLY,
    DEFERRED_INIT,
    DISPOSABLE,
    FLOATING,
    GHOST,
    HIGH_PRIORITY,
    IMMOVABLE,
    INDEPENDENT,
    INTERACTIVE,
    LAZY_UPDATE,
    LOW_PRIORITY,
    NO_ANIMATION,
    NO_CLIP,
    PASSIVE,
    PAUSED_WHEN_HIDDEN,
    PERMANENT,
    PERSISTENT,
    RESTING,
    RESOURCE_LIGHT,
    SCRIPTED,
    SLEEPING,
    UNBOUNDED,
    VOLATILE
  };
}

#endif
