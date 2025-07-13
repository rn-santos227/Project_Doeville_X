#include "GameState.h"
#include "GameStateManager.h"

#include "factories/entity/EntitiesFactory.h"
#include "libraries/keys/Keys.h"

#include <future>
#include <chrono>

namespace Project::States {
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;
  using Project::Entities::Entity;
  using Project::Handlers::ResourcesHandler;

  namespace Keys = Project::Libraries::Keys;

  GameState::GameState(SDL_Renderer* renderer, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
  : LuaScriptable(logsManager), resourcesHandler(resourcesHandler), renderer(renderer), initialized(false) {}

  GameState::~GameState() {
    clearBackground();
  }

  void GameState::initialize() {
    luaStateWrapper.registerFunction("changeState", lua_changeState, this);
    luaStateWrapper.registerFunction("setBackgroundColor", lua_setBackgroundColor, this);
    luaStateWrapper.registerFunction("setBackgroundImage", lua_setBackgroundImage, this);
    luaStateWrapper.registerFunction("spawnEntity", lua_spawnEntity, this);

    if (!luaStateWrapper.callGlobalFunction(Project::Libraries::Keys::STATE_INITIALIZE)) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'initialize'");
    }
  }

  void GameState::onEnter() {
    setActive(true);
    if (!luaStateWrapper.callGlobalFunction(Project::Libraries::Keys::STATE_ON_ENTER)) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'onEnter': " + std::string(lua_tostring(luaStateWrapper.get(), -1)));
    }
  }

  void GameState::onExit() {
    setActive(false);
    if (!luaStateWrapper.callGlobalFunction(Project::Libraries::Keys::STATE_ON_EXIT)) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'onExit': " + std::string(lua_tostring(luaStateWrapper.get(), -1)));
    }
  }

  void GameState::update(float deltaTime) {
    if (layersManager) {
      layersManager->update(deltaTime);
    } else if (entitiesManager) {
      entitiesManager->update(deltaTime);
    }

    if (!luaStateWrapper.callGlobalFunction(Project::Libraries::Keys::STATE_UPDATE)) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'update'");
    }
  }

  void GameState::render() {
    lua_State* L = luaStateWrapper.get();
    if (logsManager.checkAndLogError(!renderer, "Renderer is null.")) {
      return;
    }
  
    if (!backgroundTexture) {
      if (backgroundFuture.valid() &&
          backgroundFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        SDL_Texture* loaded = backgroundFuture.get();
        if (!loaded) {
          logsManager.logError("Failed to load background image: " + pendingBackgroundPath);
        } else {
          backgroundTexture = loaded;
          logsManager.logMessage("Background image set: " + pendingBackgroundPath);
        }
        pendingBackgroundPath.clear();
      }

      if (!backgroundTexture) {
        SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        SDL_RenderClear(renderer);
      }
    }

    if (backgroundTexture) {
      SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    }
    
    if (layersManager) {
      layersManager->render();
    } else if (entitiesManager) {
      entitiesManager->render();
    }

    if (!luaStateWrapper.callGlobalFunction(Project::Libraries::Keys::STATE_RENDER)) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'render'");
    }
  }

  void GameState::handleInput() {
    if (!luaStateWrapper.callGlobalFunction(Project::Libraries::Keys::STATE_HANDLE_INPUT)) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'handleInput'");
    }
  }

  bool GameState::setBackgroundImage(const std::string& imagePath) {
    clearBackground();

    backgroundFuture = resourcesHandler.loadTextureAsync(renderer, imagePath);
    pendingBackgroundPath = imagePath;
    useImageBackground = true;
    logsManager.logMessage("Background image loading started: " + imagePath);
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
    backgroundTexture = nullptr;
    backgroundFuture = std::future<SDL_Texture*>();
    pendingBackgroundPath.clear();
  }

  bool GameState::attachLuaScript(const std::string& scriptPath) {
    if (!LuaScriptable::attachLuaScript(scriptPath)) {
      return false;
    }

    luaStateWrapper.registerFunction(Project::Libraries::Keys::STATE_PRINT, LuaStateWrapper::luaPrintRedirect);
    logsManager.logMessage("Lua script attached: " + scriptPath);

    return true;
  }

  int Project::States::GameState::lua_setBackgroundImage(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_setBackgroundImage.");
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

  int Project::States::GameState::lua_spawnEntity(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_spawnEntity.");
    }

    const char* name = luaL_checkstring(L, 1);
    if (!name) {
      luaL_error(L, "Expected an entity name string.");
      return 0;
    }

    bool hasX = lua_gettop(L) >= 2 && lua_isnumber(L, 2);
    bool hasY = lua_gettop(L) >= 3 && lua_isnumber(L, 3);
    bool hasLayer = lua_gettop(L) >= 4 && lua_isstring(L, 4);
    float posX = 0.0f;
    float posY = 0.0f;
    if (hasX) posX = static_cast<float>(lua_tonumber(L, 2));
    if (hasY) posY = static_cast<float>(lua_tonumber(L, 3));
    std::string targetLayer;
    if (hasLayer) targetLayer = lua_tostring(L, 4);

    if (!state->entitiesFactory) {
      luaL_error(L, "EntitiesFactory not set for this state.");
      return 0;
    }

    bool isGlobal = state->entitiesFactory->isEntityGlobal(name);
    if (isGlobal && state->globalEntitiesManager && state->globalEntitiesManager->hasEntity(name)) {
      return 0;
    }

    std::unique_ptr<Entity> entity = state->entitiesFactory->cloneEntity(name);
    if (!entity) {
      luaL_error(L, ("Failed to clone entity: " + std::string(name)).c_str());
      return 0;
    }

    if (hasX) entity->getLuaStateWrapper().setGlobalNumber(Keys::X, posX);
    if (hasY) entity->getLuaStateWrapper().setGlobalNumber(Keys::Y, posY);

    entity->initialize();
    std::shared_ptr<Entity> shared = std::move(entity);

    if (isGlobal) {
      if (state->globalEntitiesManager) {
        state->globalEntitiesManager->addEntity(name, shared);
      } else {
        luaL_error(L, "Global EntitiesManager not set for this state.");
      }
    } else if (state->layersManager) {
      std::shared_ptr<Project::Entities::EntitiesManager> mgr;
      if (!targetLayer.empty()) {
        mgr = state->layersManager->getLayer(targetLayer);
      } else {
        mgr = state->layersManager->getFirstLayer();
      }
      
      if (mgr) {
        mgr->addEntity(name, shared);
      } else {
        luaL_error(L, "No layers available in LayersManager.");
      }
    } else if (state->entitiesManager) {
      state->entitiesManager->addEntity(name, shared);
    } else {
      luaL_error(L, "EntitiesManager not set for this state.");
    }

    return 0;
  }

  int Project::States::GameState::lua_changeState(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_changeState.");
    }
    if (!state->gameStateManager) {
      return luaL_error(L, "GameStateManager not set for this state.");
    }

    const char* name = luaL_checkstring(L, 1);
    if (!name) {
      luaL_error(L, "Expected a state name string.");
      return 0;
    }

    state->gameStateManager->changeState(name);
    return 0;
  }
}
