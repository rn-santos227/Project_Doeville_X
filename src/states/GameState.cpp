#include "GameState.h"

namespace Project::States {
  GameState::GameState(LogsManager& logsManager)
    : logsManager(logsManager), luaState(luaL_newstate()), initialized(false) {
    luaL_openlibs(luaState);
  }

  GameState::~GameState() {
    if (luaState) {
      lua_close(luaState);
    }
  }

  void GameState::initialize() {
    lua_getglobal(luaState, "initialize");
    if (lua_isfunction(luaState, -1)) {
      int result = lua_pcall(luaState, 0, 0, 0);
      if (result != LUA_OK) {
        handleLuaError("Error calling Lua function 'initialize': " + std::string(lua_tostring(luaState, -1)));
      } 
    } else {
      lua_pop(luaState, 1);
    }
  }

  void GameState::onEnter() {
    lua_getglobal(luaState, "onEnter");
    if (lua_isfunction(luaState, -1)) {
      int result = lua_pcall(luaState, 0, 0, 0);
      if (result != LUA_OK) {
        handleLuaError("Error calling Lua function 'onEnter': " + std::string(lua_tostring(luaState, -1)));
      } 
    } else {
      lua_pop(luaState, 1);
    }
  }

  void GameState::onExit() {
    lua_getglobal(luaState, "onExit");
    if (lua_isfunction(luaState, -1)) {
      int result = lua_pcall(luaState, 0, 0, 0);
      if (result != LUA_OK) {
        handleLuaError("Error calling Lua function 'onExit': " + std::string(lua_tostring(luaState, -1)));
      } 
    } else {
      lua_pop(luaState, 1);
    }
  }

  void GameState::update(float deltaTime) {
    lua_getglobal(luaState, "update");
    if (lua_isfunction(luaState, -1)) {
      int result = lua_pcall(luaState, 0, 0, 0);
      if (result != LUA_OK) {
        handleLuaError("Error calling Lua function 'update': " + std::string(lua_tostring(luaState, -1)));
      }
    } else {
      lua_pop(luaState, 1);
    }
  }

  void GameState::render() {
    lua_getglobal(luaState, "render");
    if (lua_isfunction(luaState, -1)) {

    }
  }

  void GameState::handleInput() {
    logsManager.logMessage("GameState update.");
  }

  bool GameState::attachLuaScript(const std::string& scriptPath) {
    if (luaL_dofile(luaState, scriptPath.c_str()) != LUA_OK) {
      handleLuaError("Failed to load Lua script: " + scriptPath);
      return false;
    }
    logsManager.logMessage("Lua script attached: " + scriptPath);
    return true;
  }

  bool GameState::callLuaFunction(const std::string& functionName) {
    lua_getglobal(luaState, functionName.c_str());

    if (!lua_isfunction(luaState, -1)) {
      handleLuaError("Lua function not found: " + functionName);
      lua_pop(luaState, 1);
      return false;
    }

    if (lua_pcall(luaState, 0, 0, 0) != LUA_OK) {
      handleLuaError("Error calling Lua function '" + functionName + "': " + std::string(lua_tostring(luaState, -1)));
      lua_pop(luaState, 1);
      return false;
    }

    logsManager.logMessage("Lua function called successfully: " + functionName);
    return true;
  }

  void GameState::handleLuaError(const std::string& errorMessage) {
    logsManager.logError(errorMessage);

    if (lua_gettop(luaState) > 0 && lua_isstring(luaState, -1)) {
      logsManager.logError("Lua error details: " + std::string(lua_tostring(luaState, -1)));
      lua_pop(luaState, 1);
    }
  }
}