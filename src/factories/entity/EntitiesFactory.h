#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include <unordered_map>

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
    explicit EntitiesFactory(LogsManager& logsManager, ComponentsFactory& componentsFactory);
    ~EntitiesFactory();

    std::unique_ptr<Entity> createEntityFromLua(const std::string& scriptPath);
    std::unique_ptr<Entity> cloneEntity(const std::string& entityName);

    bool hasEntityTemplate(const std::string& entityName) const;

  private:
    LogsManager& logsManager;    
    ComponentsFactory& componentsFactory;
    
    std::unordered_map<std::string, std::unique_ptr<Entity>> entityTemplates;
    std::unordered_map<std::string, std::string> entityScriptPaths;    
    
    std::unique_ptr<Entity> loadEntityTemplateFromLua(const std::string& scriptPath);
  };
}

#endif
