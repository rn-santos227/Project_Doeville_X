#ifndef ENTITIES_MANAGER_H
#define ENTITIES_MANAGER_H

#include "Entity.h"

#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <mutex>

namespace Project::Entities {
  class EntitiesManager {
    public:
      explicit EntitiesManager(LogsManager& logsManager);
      ~EntitiesManager();

      std::shared_ptr<Entity> createEntityFromLua(const std::string& luaScriptPath);
  };
}

#endif