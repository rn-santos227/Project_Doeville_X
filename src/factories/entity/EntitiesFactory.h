#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include <unordered_map>

#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "factories/component/ComponentsFactory.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Factories {
  class EntitiesFactory {
  public:
    explicit EntitiesFactory(Project::Utilities::LogsManager& logsManager, Project::Factories::ComponentsFactory& componentsFactory);
    ~EntitiesFactory();

    std::unique_ptr<Project::Entities::Entity> createEntityFromLua(const std::string& scriptPath);
    std::unique_ptr<Project::Entities::Entity> cloneEntity(const std::string& entityName);

    bool hasEntityTemplate(const std::string& entityName) const;

  private:
    Project::Utilities::LogsManager& logsManager;
    Project::Factories::ComponentsFactory& componentsFactory;

    std::unordered_map<std::string, std::unique_ptr<Project::Entities::Entity>> entityTemplates;
    std::unordered_map<std::string, std::string> entityScriptPaths;

    std::unique_ptr<Project::Entities::Entity> loadEntityTemplateFromLua(const std::string& scriptPath);
  };
}

#endif
