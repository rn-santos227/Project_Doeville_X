#include "EntitiesFactory.h"
#include <filesystem>

namespace Project::Factories {
  EntitiesFactory::EntitiesFactory(LogsManager& logsManager, ComponentsFactory& componentsFactory)
    : logsManager(logsManager), componentsFactory(componentsFactory) {}

  EntitiesFactory::~EntitiesFactory() {
    entityTemplates.clear();
  }

  std::unique_ptr<Entity> EntitiesFactory::createEntityFromLua(const std::string& scriptPath) {
    std::unique_ptr<Entity> entity = loadEntityTemplateFromLua(scriptPath);
    if(logsManager.checkAndLogError(!entity, "Failed to load entity template from Lua: " + scriptPath)) {
      return nullptr;
    }

    std::string name = entity->getEntityName();
    if(logsManager.checkAndLogError(name.empty(), "Entity name is empty after loading from Lua: " + scriptPath)) {
      return nullptr;
    }

    entityTemplates[name] = std::move(entity);
    entityScriptPaths[name] = scriptPath;
    return cloneEntity(name);
  }

  bool EntitiesFactory::hasEntityTemplate(const std::string& entityName) const {
    return entityTemplates.find(entityName) != entityTemplates.end();
  }

  std::unique_ptr<Entity> EntitiesFactory::cloneEntity(const std::string& entityName) {
    auto it = entityTemplates.find(entityName);
    if(logsManager.checkAndLogError(it == entityTemplates.end(), "Entity template not found: " + entityName)) {
      return nullptr;
    }

    EntityCategory category = it->second->getEntityCategory();
    std::unique_ptr<Entity> clone = std::make_unique<Entity>(category, logsManager, componentsFactory);
    clone->setEntityName(entityName);

    lua_State* templateState = it->second->getLuaState();
    if (templateState) {
      auto pathIt = entityScriptPaths.find(entityName);
      std::string scriptPath = (pathIt != entityScriptPaths.end()) ? pathIt->second : "";
    }

    return clone;
  }

  std::unique_ptr<Entity> EntitiesFactory::loadEntityTemplateFromLua(const std::string& scriptPath) {
    std::filesystem::path p(scriptPath);
    std::string name = p.stem().stem().string();

    EntityCategory category = EntityCategory::ENVIRONMENT;

    std::unique_ptr<Entity> entity = std::make_unique<Entity>(category, logsManager, componentsFactory);
    entity->setEntityName(name);

    if (logsManager.checkAndLogError(!entity->attachLuaScript(scriptPath), "Failed to attach Lua script: " + scriptPath)) {
      return nullptr;
    }

    return entity;
  }
}
