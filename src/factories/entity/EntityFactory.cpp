#include "EntityFactory.h"

namespace Project::Factories {
  EntityFactory::EntityFactory(LogsManager& logsManager) : logsManager(logsManager) {}

  EntityFactory::~EntityFactory() {
    entityTemplates.clear();
  }

  bool EntityFactory::createEntityFromLua(const std::string& scriptPath) {
    std::unique_ptr<Entity> entity = loadEntityTemplateFromLua(scriptPath);
    if(logsManager.checkAndLogError(!entity, "Failed to load entity template from Lua: " + scriptPath)) {
      logsManager.flushLogs();
      return false;
    }

    std::string name = entity->getEntityName();
    if(logsManager.checkAndLogError(name.empty(), "Entity name is empty after loading from Lua: " + scriptPath)) {
      logsManager.flushLogs();
      return false;
    }

    entityTemplates[name] = std::move(entity);
    return true;
  }

  bool EntityFactory::hasEntityTemplate(const std::string& entityName) const {
    return entityTemplates.find(entityName) != entityTemplates.end();
  }

  std::unique_ptr<Entity> EntityFactory::cloneEntity(const std::string& entityName) {
    auto it = entityTemplates.find(entityName);
    if(logsManager.checkAndLogError(it == entityTemplates.end(), "Entity template not found: " + entityName)) {
      logsManager.flushLogs();
      return nullptr;
    }

    EntityCategory category = it->second->getEntityCategory();
    std::unique_ptr<Entity> clone = std::make_unique<Entity>(category, logsManager);
    clone->setEntityName(entityName);

    lua_State* templateState = it->second->getLuaState();
    if (templateState) {
      clone->attachLuaScript("scripts/entities/" + entityName + ".lua");
    }

    return clone;
  }

  std::unique_ptr<Entity> EntityFactory::loadEntityTemplateFromLua(const std::string& scriptPath) {
    std::string fileName = scriptPath.substr(scriptPath.find_last_of("/\\") + 1);
    std::string name = fileName.substr(0, fileName.find_last_of('.'));

    EntityCategory category = EntityCategory::ENVIRONMENT;

    std::unique_ptr<Entity> entity = std::make_unique<Entity>(category, logsManager);
    entity->setEntityName(name);

    if (logsManager.checkAndLogError(!entity->attachLuaScript(scriptPath), "Failed to attach Lua script: " + scriptPath)) {
      logsManager.flushLogs();
      return nullptr;
    }

    return entity;
  }
}