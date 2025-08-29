#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include <lua.hpp>
#include <unordered_map>

#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "factories/component/ComponentsFactory.h"
#include "helpers/entity_pool/EntityPool.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/config_reader/ConfigReader.h"

namespace Project::States { class GameStateManager; }
namespace Project::Factories {  
  class EntitiesFactory {
  public:
    using EntityPtr = std::unique_ptr<Project::Entities::Entity, Project::Helpers::EntityPool::Deleter>;
    
    explicit EntitiesFactory(
      Project::Utilities::LogsManager& logsManager,
      Project::Utilities::ConfigReader& configReader,
      Project::Factories::ComponentsFactory& componentsFactory,
      Project::States::GameStateManager& gameStateManager
    );

    ~EntitiesFactory();

    EntityPtr createEntityFromLua(const std::string& scriptPath);
    EntityPtr cloneEntity(const std::string& entityName);

    bool hasEntityTemplate(const std::string& entityName) const;
    bool isEntityGlobal(const std::string& entityName) const;

  private:
    Project::Utilities::LogsManager& logsManager;
    Project::Utilities::ConfigReader& configReader;
    Project::Factories::ComponentsFactory& componentsFactory;
    Project::States::GameStateManager& gameStateManager;

    std::unordered_map<std::string, std::string> entityScriptPaths;
    std::unordered_map<std::string, EntityPtr> entityTemplates;

    EntityPtr loadEntityTemplateFromLua(const std::string& scriptPath);
  };
}

#endif
