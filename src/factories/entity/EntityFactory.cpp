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

  std::unique_ptr<Entity> EntityFactory::loadEntityTemplateFromLua(const std::string& scriptPath) {
    lua_State* L = luaL_newstate();
  }
}