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
      lua_pushnil(L);
      while (lua_next(L, -2)) {
        if (lua_isstring(L, -2) && lua_istable(L, -1)) {
          std::string key = lua_tostring(L, -2);
          float value = 0.0f;
          float limit = 0.0f;
          lua_getfield(L, -1, Keys::VALUE);
          if (lua_isnumber(L, -1)) value = static_cast<float>(lua_tonumber(L, -1));
          lua_pop(L, 1);
          lua_getfield(L, -1, Keys::LIMIT);
          if (lua_isnumber(L, -1)) limit = static_cast<float>(lua_tonumber(L, -1));
          lua_pop(L, 1);
          values[key] = {value, limit};
        }
        lua_pop(L, 1);
      }
      lua_pop(L, 1);
    } else {
      lua_pop(L, 1);
    }
  }

  void NumericComponent::setValue(const std::string& name, float value) {
    values[name].value = value;
    if (values[name].limit > 0.0f && values[name].value > values[name].limit) {
      values[name].value = values[name].limit;
    }
  }

  float NumericComponent::getValue(const std::string& name) const {
    auto it = values.find(name);
    if (it != values.end()) return it->second.value;
    return 0.0f;
  }

  void NumericComponent::setLimit(const std::string& name, float limit) {
    values[name].limit = limit;
    if (limit > 0.0f && values[name].value > limit) {
      values[name].value = limit;
    }
  }
}
