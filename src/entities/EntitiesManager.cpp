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

      void addEntity(const std::string& id, std::shared_ptr<Entity> entity);
      void removeEntity(const std::string& id);

    private:
      LogsManager& logsManager;
  };
}

#endif