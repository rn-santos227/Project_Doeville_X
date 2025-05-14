#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "entities/Entity.h"
#include "entities/EntitiesManager.h"

#include "factories/component/ComponentsFactory.h"

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Entities;
using namespace Project::Factories;
using namespace Project::Utilities;

namespace Project::Factories {
  class EntitiesFactory {
  public:
    explicit EntitiesFactory(LogsManager& logsManager);
    ~EntitiesFactory();

    bool createEntityFromLua(const std::string& scriptPath);
    bool hasEntityTemplate(const std::string& entityName) const;

    std::unique_ptr<Entity> cloneEntity(const std::string& entityName);

  private:
    LogsManager& logsManager;    
    ComponentsFactory& componentsFactory;
    
    std::unordered_map<std::string, std::unique_ptr<Entity>> entityTemplates;
    std::unique_ptr<Entity> loadEntityTemplateFromLua(const std::string& scriptPath);
  };
}

#endif