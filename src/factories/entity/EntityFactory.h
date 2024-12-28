#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "entities/Entity.h"
#include "entities/EntitiesManager.h"

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Entities;
using namespace Project::Utilities;

namespace Project::Factories {
  class EntityFactory {
  public:
    explicit EntityFactory(EntitiesManager& entitiesManager, LogsManager& logsManager);
    
    bool createEntityFromLua(const std::string& scriptPath);

  private:

  };
}

#endif