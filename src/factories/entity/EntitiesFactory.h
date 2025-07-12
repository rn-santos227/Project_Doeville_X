#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include <lua.hpp>
#include <unordered_map>

#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "factories/component/ComponentsFactory.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::States { class GameStateManager; }
namespace Project::Factories {
  class EntitiesFactory {
  public:
    explicit EntitiesFactory(Project::Utilities::LogsManager& logsManager, Project::Factories::ComponentsFactory& componentsFactory, Project::States::GameStateManager& gameStateManager);
    ~EntitiesFactory();

    std::unique_ptr<Project::Entities::Entity> createEntityFromLua(const std::string& scriptPath);
    std::unique_ptr<Project::Entities::Entity> cloneEntity(const std::string& entityName);

    bool hasEntityTemplate(const std::string& entityName) const;
    bool isEntityGlobal(const std::string& entityName) const;

  private:
    Project::Utilities::LogsManager& logsManager;
    Project::Factories::ComponentsFactory& componentsFactory;
    Project::States::GameStateManager& gameStateManager;

    std::unordered_map<std::string, std::unique_ptr<Project::Entities::Entity>> entityTemplates;
    std::unordered_map<std::string, std::string> entityScriptPaths;

    std::unique_ptr<Project::Entities::Entity> loadEntityTemplateFromLua(const std::string& scriptPath);

    static int lua_changeState(lua_State* L);
  };
}

#endif
