#ifndef ENTITY_ID_H
#define ENTITY_ID_H

namespace Project::Entities {
  enum class EntityID {
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