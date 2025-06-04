#ifndef ENTITY_CATEGORY_H
#define ENTITY_CATEGORY_H

namespace Project::Entities {
  enum class EntityCategory {
    PLAYER,
    ENEMY,
    NPC,
    PROJECTILE,
    OBSTACLE,
    VEHICLE,
    TRIGGER,
    ENVIRONMENT,
    PICKUP,
    DOOR,
    CHECKPOINT,
    CAMERA,
    UI_ELEMENT  
  };
}

#endif
