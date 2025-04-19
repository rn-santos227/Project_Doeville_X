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
    luaL_openlibs(L);

    if (luaL_dofile(L, scriptPath.c_str()) != LUA_OK) {
      logsManager.logError("Lua error in script: " + scriptPath + "\n" + lua_tostring(L, -1));
      lua_close(L);
      return nullptr;
    }

    lua_getglobal(L, "entityID");
    if (!lua_isinteger(L, -1)) {
      logsManager.logError("Lua script is missing a valid 'entityID': " + scriptPath);
      lua_close(L);
      return nullptr;
    }

    lua_pop(L, 1);
  }

  std::unique_ptr<Entity> EntityFactory::loadEntityTemplateFromLua(const std::string& scriptPath) {
    std::string fileName = scriptPath.substr(scriptPath.find_last_of("/\\") + 1);
    std::string name = fileName.substr(0, fileName.find_last_of('.'));

    EntityCategory category = EntityCategory::ENVIRONMENT;

    std::unique_ptr<Entity> entity = std::make_unique<Entity>(category, logsManager);
  }
}