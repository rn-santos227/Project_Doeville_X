#include "GameState.h"

namespace Project::States {
  GameState::GameState(SDL_Renderer* renderer, LogsManager& logsManager)
    : logsManager(logsManager), renderer(renderer), luaState(luaL_newstate()), initialized(false) {
    luaL_openlibs(luaState);
  }

  GameState::~GameState() {
    if (luaState) {
      lua_close(luaState);
    }
    clearBackground();
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

  void GameState::initialize() {
    lua_pushlightuserdata(luaState, this);
    lua_pushcclosure(luaState, lua_setBackgroundColor, 1);
    lua_setglobal(luaState, "setBackgroundColor");

    if (!callLuaFunction("initialize")) {
      handleLuaError("Error calling Lua function 'initialize'");
    }
  }

  void GameState::onEnter() {
    if (!callLuaFunction("onEnter")) {
      handleLuaError("Error calling Lua function 'onEnter': " + std::string(lua_tostring(luaState, -1)));
    }
  }

  void GameState::onExit() {
    if (!callLuaFunction("onExit")) {
      handleLuaError("Error calling Lua function 'onExit': " + std::string(lua_tostring(luaState, -1)));
    }
  }

  void GameState::update(float deltaTime) {
    if (entitiesManager) {
      entitiesManager->update(deltaTime);
    }

    if (!callLuaFunction("update")) {
      handleLuaError("Error calling Lua function 'update'");
    }
  }

  void GameState::render(SDL_Renderer* renderer) {
    if (logsManager.checkAndLogError(!renderer, "Renderer is null.")) {
      logsManager.flushLogs();
      return;
    }
  
    if (!backgroundTexture) {
      SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
      SDL_RenderClear(renderer);
    } else {
      SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    }

    if (entitiesManager) {
      entitiesManager->render();
    }

    lua_getglobal(luaState, "render");
  }

  void GameState::handleInput() {
    lua_getglobal(luaState, "handleInput");
    if (lua_isfunction(luaState, -1)) {
      int result = lua_pcall(luaState, 0, 0, 0);
      if (result != LUA_OK) {
        handleLuaError("Error calling Lua function 'handleInput': " + std::string(lua_tostring(luaState, -1)));
      }
    } else {
      lua_pop(luaState, 1);
    }
  }

  bool GameState::setBackgroundImage(const std::string& imagePath) {
    clearBackground();

    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if(logsManager.checkAndLogError(!surface, "Failed to load background image: " + std::string(IMG_GetError()))) {
      logsManager.flushLogs();
      return false;
    }

    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if(logsManager.checkAndLogError(!backgroundTexture, "Failed to create texture from surface: " + std::string(SDL_GetError()))) {
      logsManager.flushLogs();
      return false;
    }

    logsManager.logMessage("Background image set: " + imagePath);
    return true;
  }

  void GameState::setBackgroundColor(Uint8 r, Uint8 g, Uint8 b) {
    clearBackground();
    backgroundColor = {r, g, b, 255};
    useImageBackground = false;
    logsManager.logMessage("Background color set to RGB(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ")");
  }

  void GameState::clearBackground() {
    if (backgroundTexture) {
      SDL_DestroyTexture(backgroundTexture);
      backgroundTexture = nullptr;
    }
  }

  bool GameState::attachLuaScript(const std::string& scriptPath) {
    if (luaL_dofile(luaState, scriptPath.c_str()) != LUA_OK) {
      handleLuaError("Failed to load Lua script: " + scriptPath);
      return false;
    }
    logsManager.logMessage("Lua script attached: " + scriptPath);
    return true;
  }

  void GameState::handleLuaError(const std::string& errorMessage) {
    logsManager.logError(errorMessage);

    if (lua_gettop(luaState) > 0 && lua_isstring(luaState, -1)) {
      logsManager.logError("Lua error details: " + std::string(lua_tostring(luaState, -1)));
      lua_pop(luaState, 1);
    }
  }

  int Project::States::GameState::lua_setBackgroundImage(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      luaL_error(L, "Invalid GameState reference in lua_setBackgroundImage.");
      return 0;
    }
  }

  int GameState::lua_setBackgroundColor(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    Uint8 r = static_cast<Uint8>(luaL_checkinteger(L, 1));
    Uint8 g = static_cast<Uint8>(luaL_checkinteger(L, 2));
    Uint8 b = static_cast<Uint8>(luaL_checkinteger(L, 3));
    state->setBackgroundColor(r, g, b);
    return 0;
  }
}