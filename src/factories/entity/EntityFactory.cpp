#include "EntityFactory.h"

namespace Project::Factories {
  EntityFactory::EntityFactory(LogsManager& logsManager) : logsManager(logsManager) {}

  EntityFactory::~EntityFactory() {
    entityTemplates.clear();
  }

  bool EntityFactory::createEntityFromLua(const std::string& scriptPath) {

  }

  bool EntityFactory::hasEntityTemplate(const std::string& entityName) const {
    return entityTemplates.find(entityName) != entityTemplates.end();
  }

  std::unique_ptr<Entity> EntityFactory::cloneEntity(const std::string& entityName) {

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