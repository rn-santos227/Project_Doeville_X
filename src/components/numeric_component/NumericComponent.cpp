#include "NumericComponent.h"

#include "libraries/keys/Keys.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;

  namespace Keys = Project::Libraries::Keys;

  NumericComponent::NumericComponent(LogsManager& logsManager)
    : BaseComponent(logsManager) {}

  void NumericComponent::update(float /*deltaTime*/) {}

  void NumericComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    lua_State* L = luaStateWrapper.get();
    lua_getglobal(L, tableName.c_str());
    if (lua_istable(L, -1)) {

    } else {
      lua_pop(L, 1);
    }
  }
}
