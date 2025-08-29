#include "KeysComponent.h"

#include "entities/Entity.h"
#include "handlers/input/KeyActionResolver.h"
#include "handlers/input/KeyCodeResolver.h"
#include "libraries/keys/Keys.h"


namespace Project::Components {
  using Project::Handlers::KeyAction;
  using Project::Handlers::KeyHandler;

  namespace Keys = Project::Libraries::Keys;

  KeysComponent::KeysComponent(Project::Utilities::LogsManager& logsManager, KeyHandler* keyHandler)
    : BaseComponent(logsManager), keyHandler(keyHandler) {}

  void KeysComponent::update(float deltaTime) {
    if (!keyHandler || !owner) return;

    for (const auto& [action, func] : functionCallbacks) {
      if (keyHandler->isActionTriggered(action)) {
        func();
      }
    }

    for (const auto& [action, func] : actionCallbacks) {
      if (keyHandler->isActionTriggered(action)) {
        owner->callLuaFunction(func);
      }
    }
  }

  void KeysComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    lua_State* L = luaStateWrapper.get();
    lua_getglobal(L, tableName.c_str());
    if (lua_istable(L, -1)) {
      lua_getfield(L, -1, Keys::BINDINGS);
      if (lua_istable(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
          if (lua_isstring(L, -2) && lua_isstring(L, -1)) {
            std::string actionName = lua_tostring(L, -2);
            std::string funcName = lua_tostring(L, -1);
            auto action = Project::Handlers::KeyActionResolver::resolve(actionName);
            if (action != Project::Handlers::KeyAction::NONE) {
              addActionCallback(action, funcName);
            }
          }
          lua_pop(L, 1);
        }
        lua_pop(L, 1);
      } else {
        lua_pop(L, 1);
      }

      lua_getfield(L, -1, Keys::KEYS);
      if (lua_istable(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
          if (lua_istable(L, -1)) {
            std::string keyStr;
            std::string actionName;
            lua_getfield(L, -1, Keys::KEY);
            if (lua_isstring(L, -1)) keyStr = lua_tostring(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::ACTION);
            if (lua_isstring(L, -1)) actionName = lua_tostring(L, -1);
            lua_pop(L, 1);

            SDL_Scancode code = Project::Handlers::KeyCodeResolver::resolve(keyStr);
            auto action = Project::Handlers::KeyActionResolver::resolve(actionName);

            if (code != SDL_SCANCODE_UNKNOWN && action != Project::Handlers::KeyAction::NONE && keyHandler) {
              keyHandler->setKeyBinding(action, code);
            }
          }
          lua_pop(L, 1);
        }
        lua_pop(L, 1);
      } else {
        lua_pop(L, 1);
      }

      lua_pop(L, 1);
    } else {
      lua_pop(L, 1);
    }
  }

  bool KeysComponent::isActionTriggered(KeyAction action) const {
    return keyHandler ? keyHandler->isActionTriggered(action) : false;
  }

  bool KeysComponent::isKeyPressed(SDL_Scancode key) const {
    return keyHandler ? keyHandler->isKeyPressed(key) : false;
  }

  void KeysComponent::addActionCallback(KeyAction action, const std::string& functionName) {
    actionCallbacks[action] = functionName;
  }

  void KeysComponent::addActionCallback(KeyAction action, std::function<void()> callback) {
    functionCallbacks[action] = std::move(callback);
  }
}
