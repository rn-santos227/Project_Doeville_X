#include "GameState.h"
#include "GameStateManager.h"

#include <algorithm>
#include <fstream>
#include <chrono>
#include <future>
#include <sstream>
#include <string>

#include "components/motion_component/MotionComponent.h"
#include "bindings/LuaBindings.h"
#include "factories/entity/EntitiesFactory.h"
#include "libraries/keys/Keys.h"
#include "libraries/constants/FloatConstants.h"
#include "libraries/constants/Constants.h"

namespace Project::States {
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;
  using Project::Entities::Entity;
  using Project::Handlers::ResourcesHandler;

  namespace Keys = Project::Libraries::Keys;
  namespace LuaBindings = Project::Bindings::LuaBindings;

  std::unordered_map<std::string, std::vector<std::string>> GameState::scriptFunctionCache{};
  Project::Utilities::BinaryFileCache GameState::persistentFunctionCache(Project::Libraries::Constants::SCRIPT_FUNCTION_CACHE_FILE);

  GameState::GameState(SDL_Renderer* renderer, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
  : LuaScriptable(logsManager), resourcesHandler(resourcesHandler), renderer(renderer), initialized(false) {}

  GameState::~GameState() {
    clearBackground();
    persistentFunctionCache.save();
  }

  void GameState::initialize() {
    registerLuaFunctions();
    if (!luaStateWrapper.callGlobalFunction(Project::Libraries::Keys::STATE_INITIALIZE)) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'initialize'");
    }
    ensureMapSize();
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
    ensureMapSize();
    
    auto* camHandler = Project::Components::GraphicsComponent::getCameraHandler();
    if (camHandler) {
      int offsetX = camHandler->getWidth();
      int offsetY = camHandler->getHeight();
      camHandler->setCullingOffset(offsetX, offsetY);
    }
    
    if (layersManager) {
      layersManager->update(deltaTime);
    } else if (entitiesManager) {
      entitiesManager->update(deltaTime);
    }

    if (dimensionMode == DimensionMode::BOXED && camHandler) {
      SDL_Rect camRect = camHandler->getRect();
      if (layersManager) {
        layersManager->clampEntitiesToRect(camRect);
      } else if (entitiesManager) {
        entitiesManager->clampEntitiesToRect(camRect);
      }
    } else if (dimensionMode == DimensionMode::BOUNDED) {
      if (mapRect.w > 0 && mapRect.h > 0) {
        if (layersManager) {
          layersManager->clampEntitiesToRect(mapRect);
        } else if (entitiesManager) {
          entitiesManager->clampEntitiesToRect(mapRect);
        }
      }
    }

    for (auto& pair : entitySeeders) {
      if (pair.second) pair.second->update(deltaTime);
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

  void GameState::reset() {
    clearScriptFunctionCache();
    playerEntity.reset();
    activeCamera = nullptr;
    nextSeederId = 0;
    lastSeederId.clear();
    entitySeeders.clear();

    if (layersManager) {
      layersManager->reset();
    }
    if (entitiesManager) {
      entitiesManager->reset();
    }

    clearBackground();

    luaStateWrapper.reset();
    if (!luaScriptPath.empty()) {
      luaStateWrapper.loadScript(luaScriptPath);
    }

    initialize();
    markInitialized();
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

  void GameState::setActiveCamera(Project::Components::CameraComponent* camera) {
    if (activeCamera) activeCamera->setActive(false);
    activeCamera = camera;
    if (activeCamera) activeCamera->setActive(true);
  }

  std::shared_ptr<Project::Entities::Entity> GameState::findEntity(const std::string& name) {
    if (layersManager) {
      auto ent = layersManager->findEntity(name);
      if (ent) return ent;
    } else if (entitiesManager) {
      auto ent = entitiesManager->getEntity(name);
      if (ent) return ent;
    }
    if (globalEntitiesManager) {
      return globalEntitiesManager->getEntity(name);
    }
    return nullptr;
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

  std::string GameState::startEntitySeeder(const std::string& seed, const std::string& layer, const std::string& id, Project::Entities::ChunkSize chunkSize) {
    std::shared_ptr<Project::Entities::EntitiesManager> mgr;
    if (layersManager) {
      if (!layer.empty()) {
        mgr = layersManager->getLayer(layer);
      }
      if (!mgr) {
        mgr = layersManager->getFirstLayer();
      }
    } else {
      mgr = entitiesManager;
    }

    if (!mgr || !entitiesFactory) return "";

    int screenW = 0;
    int screenH = 0;
    if (renderer) {
      SDL_GetRendererOutputSize(renderer, &screenW, &screenH);
    }
    if (chunkSize.x <= 0.0f) chunkSize.x = static_cast<float>(screenW);
    if (chunkSize.y <= 0.0f) chunkSize.y = static_cast<float>(screenH);

    auto seeder = std::make_unique<Project::Entities::EntitySeeder>(*mgr, *entitiesFactory);
    seeder->setChunkSize(chunkSize);

    if (!seed.empty()) {
      bool numeric = std::all_of(seed.begin(), seed.end(), ::isdigit);
      if (numeric) {
        seeder->setSeed(static_cast<size_t>(std::stoull(seed)));
      } else {
        seeder->setSeedString(seed);
      }
    }

    auto playerEntity = getPlayerEntity();
    if (!playerEntity) playerEntity = findEntity(Keys::PLAYER);

    if (playerEntity) seeder->setPlayer(playerEntity);

    return addEntitySeeder(std::move(seeder), id);
  }

  std::string GameState::addEntitySeeder(std::unique_ptr<Project::Entities::EntitySeeder> seeder, const std::string& id) {
    std::string finalId = id;
    if (finalId.empty()) {
      do {
        finalId = "seeder_" + std::to_string(nextSeederId++);
      } while (entitySeeders.count(finalId));
    } else if (entitySeeders.count(finalId)) {
      entitySeeders.erase(finalId);
    }
    lastSeederId = finalId;
    entitySeeders[finalId] = std::move(seeder);
    return finalId;
  }

  void GameState::addEntityToSeed(const std::string& name, const std::string& seederId, size_t count) {
    if (entitySeeders.empty()) return;
    std::string id = seederId.empty() ? lastSeederId : seederId;
    auto it = entitySeeders.find(id);
    if (it != entitySeeders.end() && it->second) {
      it->second->addEntityTemplate(name, count);
    }
  }

  void GameState::setPlayerEntity(const std::string& name, float x, float y, bool setPosition) {
    playerEntity.reset();
    auto ent = findEntity(name);
    if (ent) {
      playerEntity = ent;
      if (setPosition) {
        if (dimensionMode == DimensionMode::BOUNDED) {
          ensureMapSize();
          float maxX = static_cast<float>(mapRect.w);
          float maxY = static_cast<float>(mapRect.h);
          x = std::clamp(x, 0.0f, maxX);
          y = std::clamp(y, 0.0f, maxY);
        }
        ent->setPosition(x, y);
      }
    }
  }

  std::shared_ptr<Project::Entities::Entity> GameState::getPlayerEntity() const {
    return playerEntity.lock();
  }

  size_t GameState::getEntityCount() const {
    size_t count = 0;
    if (layersManager) {
      count += layersManager->getTotalEntityCount();
    } else if (entitiesManager) {
      count += entitiesManager->getEntityCount();
    }
    if (globalEntitiesManager) {
      count += globalEntitiesManager->getEntityCount();
    }
    return count;
  }
  
  void GameState::registerLuaFunctions(Project::Core::SDLManager* manager) {
    if (manager) sdlManager = manager;
    const std::string& path = luaScriptPath;
    if (path.empty()) return;

    std::vector<std::string> funcs;
    auto it = scriptFunctionCache.find(path);
    if (it == scriptFunctionCache.end()) {
      std::vector<char> raw;
      if (persistentFunctionCache.getData(path, raw)) {
        std::string content(raw.begin(), raw.end());
        std::istringstream iss(content);
        std::string f;
        while (std::getline(iss, f)) {
          if (!f.empty()) funcs.push_back(f);
        }
      } else {
        std::string code;
        std::ifstream in(path);
        if (in) {
          std::ostringstream ss; ss << in.rdbuf(); code = ss.str();
        }
        auto contains = [&](const std::string& name){ return code.find(name) != std::string::npos; };
        auto record = [&](const std::string& name){ if (contains(name)) funcs.push_back(name); };
        record(Keys::LUA_SET_ACTIVE_CAMERA);
        record(Keys::LUA_CHANGE_STATE);
        record(Keys::LUA_RESET_STATE);
        record(Keys::LUA_SET_BACKGROUND_COLOR);
        record(Keys::LUA_SET_BACKGROUND_IMAGE);
        record(Keys::LUA_SPAWN_ENTITY);
        record(Keys::LUA_START_ENTITY_SEEDER);
        record(Keys::LUA_ADD_ENTITY_TO_SEEDER);
        record(Keys::LUA_SET_PLAYER_ENTITY);
        record(Keys::LUA_SET_MAP_SIZE);
        record(Keys::LUA_EXIT_GAME);
        std::string serialized;
        for (const auto& f : funcs) serialized += f + "\n";
        persistentFunctionCache.setData(path, std::vector<char>(serialized.begin(), serialized.end()));
      }
      scriptFunctionCache[path] = funcs;
    } else {
      funcs = it->second;
    }

    for (const auto& f : funcs) {
      if (f == Keys::LUA_SET_ACTIVE_CAMERA) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_setActiveCamera, this);
      } else if (f == Keys::LUA_CHANGE_STATE) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_changeState, this);
      } else if (f == Keys::LUA_RESET_STATE) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_resetState, this);
      } else if (f == Keys::LUA_SET_BACKGROUND_COLOR) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_setBackgroundColor, this);
      } else if (f == Keys::LUA_SET_BACKGROUND_IMAGE) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_setBackgroundImage, this);
      } else if (f == Keys::LUA_SPAWN_ENTITY) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_spawnEntity, this);
      } else if (f == Keys::LUA_START_ENTITY_SEEDER) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_startEntitySeeder, this);
      } else if (f == Keys::LUA_ADD_ENTITY_TO_SEEDER) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_addEntityToSeed, this);
      } else if (f == Keys::LUA_SET_PLAYER_ENTITY) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_setPlayerEntity, this);
      } else if (f == Keys::LUA_SET_MAP_SIZE) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_setMapSize, this);
      } else if (f == Keys::LUA_EXIT_GAME && sdlManager) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_exitGame, sdlManager);
      }
    }
  }

  void GameState::clearScriptFunctionCache() {
    scriptFunctionCache.clear();
    persistentFunctionCache.load();
  }

  void GameState::ensureMapSize() {
    if (dimensionMode != DimensionMode::BOUNDED) return;
    if (mapRect.w > 0 && mapRect.h > 0) return;

    mapRect.x = 0;
    mapRect.y = 0;
    if (renderer) {
      SDL_GetRendererOutputSize(renderer, &mapRect.w, &mapRect.h);
    }
    if (mapRect.w <= 0 || mapRect.h <= 0) {
      mapRect.w = Project::Libraries::Constants::DEFAULT_SCREEN_WIDTH;
      mapRect.h = Project::Libraries::Constants::DEFAULT_SCREEN_HEIGHT;
    }
  }
}
