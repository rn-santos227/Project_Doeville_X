#include "Entity.h"

namespace Project::Entities {
  Entity::Entity(EntityID id, LogsManager& logsManager)
    : entityID(id), logsManager(logsManager), luaState(luaL_newstate()) {

    }
}