#include "GameState.h"

namespace Project::States {
  GameState::GameState(SDL_Renderer* renderer, LogsManager& logsManager)
    : logsManager(logsManager), renderer(renderer), luaStateWrapper(logsManager), initialized(false) {
    if (logsManager.checkAndLogError(!luaStateWrapper.isValid(), "Failed to create Lua state")) {
      logsManager.flushLogs();
      return;
    }

    lua_State* L = luaStateWrapper.get();
    luaL_openlibs(L);

    lua_pushcfunction(L, luaStateWrapper.luaPrintRedirect);
    lua_setglobal(L, "print");
  }

  GameState::~GameState() {
    clearBackground();
  }

  bool GameState::callLuaFunction(const std::string& functionName) {
    return luaStateWrapper.callGlobalFunction(functionName);
  }

  void GameState::initialize() {
    lua_State* L = luaStateWrapper.get();
    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, lua_setBackgroundColor, 1);
    lua_setglobal(L, "setBackgroundColor");

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, lua_setBackgroundImage, 1);
    lua_setglobal(L, "setBackgroundImage");

    if (!luaStateWrapper.callGlobalFunction("initialize")) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'initialize'");
    }
  }

  void GameState::onEnter() {
    setActive(true);
    if (!luaStateWrapper.callGlobalFunction("onEnter")) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'onEnter': " + std::string(lua_tostring(luaStateWrapper.get(), -1)));
    }
  }

  void GameState::onExit() {
    setActive(false);
    if (!luaStateWrapper.callGlobalFunction("onExit")) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'onExit': " + std::string(lua_tostring(luaStateWrapper.get(), -1)));
    }
  }

  void GameState::update(float deltaTime) {
    if (entitiesManager) {
      entitiesManager->update(deltaTime);
    }

    if (!luaStateWrapper.callGlobalFunction("update")) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'update'");
    }
  }

  void GameState::render() {
    lua_State* L = luaStateWrapper.get();
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

    if (!luaStateWrapper.callGlobalFunction("render")) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'render'");
    }
  }

  void GameState::handleInput() {
    if (!luaStateWrapper.callGlobalFunction("handleInput")) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'handleInput'");
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

  void GameState::setBackgroundColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    clearBackground();
    backgroundColor = {r, g, b, a};
    useImageBackground = false;
    logsManager.logMessage(
      "Background color set to RGBA(" +
      std::to_string(r) + ", " +
      std::to_string(g) + ", " +
      std::to_string(b) + ", " +
      std::to_string(a) + ")"
    );
  }

  void GameState::clearBackground() {
    if (backgroundTexture) {
      SDL_DestroyTexture(backgroundTexture);
      backgroundTexture = nullptr;
    }
  }

  bool GameState::attachLuaScript(const std::string& scriptPath) {
    if (logsManager.checkAndLogError(!luaStateWrapper.isValid(), "Lua state is null for GameState: " + stateName)) {
      logsManager.flushLogs();
      return false;
    }

    if (logsManager.checkAndLogError(!luaStateWrapper.loadScript(scriptPath), "Failed to load Lua script: " + scriptPath)) {
      logsManager.flushLogs();
      return false;
    }

    lua_State* L = luaStateWrapper.get();
    luaL_openlibs(L);

    lua_pushcfunction(L, luaStateWrapper.luaPrintRedirect);
    lua_setglobal(L, "print");
    
    logsManager.logMessage("Lua script attached: " + scriptPath);
    return true;
  }

  int Project::States::GameState::lua_setBackgroundImage(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      luaL_error(L, "Invalid GameState reference in lua_setBackgroundImage.");
      return 0;
    }

    const char* path = luaL_checkstring(L, 1);
    if (!path) {
      luaL_error(L, "Expected a valid image path string.");
      return 0;
    }

    if (!state->setBackgroundImage(path)) {
      luaL_error(L, ("Failed to set background image: " + std::string(path)).c_str());
    }

    return 0;
  }

  int Project::States::GameState::lua_setBackgroundColor(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    Uint8 r = static_cast<Uint8>(luaL_checkinteger(L, 1));
    Uint8 g = static_cast<Uint8>(luaL_checkinteger(L, 2));
    Uint8 b = static_cast<Uint8>(luaL_checkinteger(L, 3));
    Uint8 a = static_cast<Uint8>(luaL_optinteger(L, 4, 255));
    state->setBackgroundColor(r, g, b, a);
    return 0;
  }
}