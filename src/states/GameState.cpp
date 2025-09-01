#include "GameState.h"
#include "GameStateManager.h"

#include <algorithm>
#include <fstream>
#include <chrono>
#include <future>
#include <random>
#include <sstream>
#include <string>

#include "components/motion_component/MotionComponent.h"
#include "components/vision_component/VisionComponent.h"
#include "bindings/LuaBindings.h"
#include "factories/entity/EntitiesFactory.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::States {
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;
  using Project::Entities::Entity;
  using Project::Handlers::ResourcesHandler;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;
  namespace LuaBindings = Project::Bindings::LuaBindings;

  std::unordered_map<std::string, std::vector<std::string>> GameState::scriptFunctionCache{};
  Project::Utilities::BinaryFileCache GameState::persistentFunctionCache(Constants::SCRIPT_FUNCTION_CACHE_FILE);

  GameState::GameState(SDL_Renderer* renderer, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
  : LuaScriptable(logsManager), resourcesHandler(resourcesHandler), renderer(renderer) {}

  GameState::~GameState() {
    clearBackground();
    persistentFunctionCache.save();
  }

  void GameState::initialize() {
    registerLuaFunctions();
    if (!luaStateWrapper.callGlobalFunction(Keys::STATE_INITIALIZE)) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'initialize'");
    }
    ensureMapSize();
    updateDayNightCycle(0.0f);
  }

  void GameState::onEnter() {
    setActive(true);
    if (!luaStateWrapper.callGlobalFunction(Keys::STATE_ON_ENTER)) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'onEnter': " + std::string(lua_tostring(luaStateWrapper.get(), -1)));
    }
  }

  void GameState::onExit() {
    setActive(false);
    if (!luaStateWrapper.callGlobalFunction(Keys::STATE_ON_EXIT)) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'onExit': " + std::string(lua_tostring(luaStateWrapper.get(), -1)));
    }
  }

  void GameState::update(float deltaTime) {
    ensureMapSize();
    updateDayNightCycle(deltaTime);

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

    if (data.dimensionMode == DimensionMode::BOXED && camHandler) {
      SDL_FRect camRectF = camHandler->getRect();
      SDL_Rect camRect{static_cast<int>(camRectF.x), static_cast<int>(camRectF.y), static_cast<int>(camRectF.w), static_cast<int>(camRectF.h)};
      if (layersManager) {
        layersManager->clampEntitiesToRect(camRect);
      } else if (entitiesManager) {
        entitiesManager->clampEntitiesToRect(camRect);
      }
    } else if (data.dimensionMode == DimensionMode::BOUNDED || data.dimensionMode == DimensionMode::MAPPED) {
      if (data.mapRect.w > 0 && data.mapRect.h > 0) {
        if (layersManager) {
          layersManager->clampEntitiesToRect(data.mapRect);
        } else if (entitiesManager) {
          entitiesManager->clampEntitiesToRect(data.mapRect);
        }
      }
    }

    for (auto& pair : entitySeeders) {
      if (pair.second) pair.second->update(deltaTime);
    }

    if (!luaStateWrapper.callGlobalFunction(Keys::STATE_UPDATE)) {
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
        SDL_SetRenderDrawColor(
          renderer,
          data.backgroundColor.r,
          data.backgroundColor.g,
          data.backgroundColor.b,
          data.backgroundColor.a
        );
        SDL_RenderClear(renderer);
      }
    }

    if (backgroundTexture) {
      SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    }
    
    auto* camHandler = Project::Components::GraphicsComponent::getCameraHandler();
    float zoom = Constants::DEFAULT_CAMERA_ZOOM;
    SDL_FRect viewport{0.0f, 0.0f, 0.0f, 0.0f};
    SDL_FRect camRect{0.0f, 0.0f, 0.0f, 0.0f};
    float camX = 0.0f;
    float camY = 0.0f;
    bool useCull = false;
    if (camHandler) {
      zoom = camHandler->getZoom();
      camX = camHandler->getX();
      camY = camHandler->getY();
      viewport = SDL_FRect{0.0f, 0.0f, static_cast<float>(camHandler->getViewportWidth()), static_cast<float>(camHandler->getViewportHeight())};
      camRect = camHandler->getRect();
      useCull = true;
    }

    for (const auto& tile : mapTiles) {
      if (!tile.texture) continue;
      SDL_FRect worldRect{static_cast<float>(tile.dest.x), static_cast<float>(tile.dest.y), static_cast<float>(tile.dest.w), static_cast<float>(tile.dest.h)};
      if (useCull && !SDL_HasIntersectionF(&worldRect, &camRect)) {
        continue;
      }

      SDL_FRect dest = worldRect;
      if (useCull) {
        dest.x = (worldRect.x - camX) * zoom;
        dest.y = (worldRect.y - camY) * zoom;
        dest.w = worldRect.w * zoom;
        dest.h = worldRect.h * zoom;
        if (!SDL_HasIntersectionF(&dest, &viewport)) continue;
      }
      SDL_RenderCopyF(renderer, tile.texture, &tile.src, &dest);
    }

    if (layersManager) {
      layersManager->render();
    } else if (entitiesManager) {
      entitiesManager->render();
    }

    if (renderer && data.darkness > Constants::ANGLE_0_DEG) {
      int w = 0, h = 0;
      SDL_GetRendererOutputSize(renderer, &w, &h);
      SDL_Texture* mask = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
      if (mask) {
        SDL_SetTextureBlendMode(mask, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, mask);
        SDL_SetRenderDrawColor(
          renderer,
          Constants::COLOR_BLACK.r,
          Constants::COLOR_BLACK.g,
          Constants::COLOR_BLACK.b,
          static_cast<Uint8>(data.darkness * Constants::FLOAT_255)
        );
        SDL_RenderClear(renderer);

        #ifdef SDL_BLENDMODE_MIN
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MIN);
        #elif defined(SDL_BLENDOPERATION_MINIMUM)
        SDL_BlendMode minBlend = SDL_ComposeCustomBlendMode(
          SDL_BLENDFACTOR_ONE,
          SDL_BLENDFACTOR_ONE,
          SDL_BLENDOPERATION_MINIMUM,
          SDL_BLENDFACTOR_ONE,
          SDL_BLENDFACTOR_ONE,
          SDL_BLENDOPERATION_MINIMUM
        );
        SDL_SetRenderDrawBlendMode(renderer, minBlend);
        #else
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        #endif

        if (layersManager) {
          layersManager->renderVisionMask(renderer);
        } else if (entitiesManager) {
          auto& comps = entitiesManager->getComponentArray(Project::Components::ComponentType::VISION);
          for (auto* base : comps) {
            auto* vision = dynamic_cast<Project::Components::VisionComponent*>(base);
            if (vision) vision->renderMask(renderer);
          }
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderCopy(renderer, mask, nullptr, nullptr);
        SDL_DestroyTexture(mask);
      }
    }

    if (!luaStateWrapper.callGlobalFunction(Keys::STATE_RENDER)) {
      luaStateWrapper.handleLuaError("Error calling Lua function 'render'");
    }
  }

  void GameState::renderDebug() {
    for (auto& pair : entitySeeders) {
      if (pair.second) {
        pair.second->renderDebug(renderer);
      }
    }
  }

  void GameState::reset() {
    clearScriptFunctionCache();
    playerEntity.reset();
    activeCamera = nullptr;
    nextSeederId = 0;
    seedIndex = 0;
    lastSeederId.clear();
    entitySeeders.clear();
    clearEntityScriptOverrides();

    if (layersManager) {
      layersManager->reset();
    }
    if (entitiesManager) {
      entitiesManager->reset();
    }

    clearBackground();
    mapTiles.clear();

    luaStateWrapper.reset();
    if (!luaScriptPath.empty()) {
      luaStateWrapper.loadScript(luaScriptPath);
    }

    initialize();
    markInitialized();
  }

  void GameState::handleInput() {
    if (!luaStateWrapper.callGlobalFunction(Keys::STATE_HANDLE_INPUT)) {
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
    data.backgroundColor = {r, g, b, a};
    useImageBackground = false;
    logsManager.logMessage(
      "Background color set to RGBA(" +
      std::to_string(r) + ", " +
      std::to_string(g) + ", " +
      std::to_string(b) + ", " +
      std::to_string(a) + ")"
    );
  }

  void GameState::setDarkness(float value) {
    data.darkness = std::clamp(
      value,
      Constants::ANGLE_0_DEG,
      Constants::DEFAULT_WHOLE
    );
  }

  void GameState::setLayerDarkness(const std::string& name, float value) {
    if (layersManager) {
      layersManager->setLayerDarkness(name, value);
    }
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

    luaScriptPath = scriptPath;
    luaStateWrapper.registerFunction(Keys::STATE_PRINT, LuaStateWrapper::luaPrintRedirect);
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

    std::string actualSeed = seed;
    if (actualSeed.empty()) {
      if (seedIndex < seederSeeds.size()) {
        actualSeed = seederSeeds[seedIndex];
      } else {
        std::random_device rd;
        actualSeed = std::to_string(rd());
        seederSeeds.push_back(actualSeed);
      }
    }
    ++seedIndex;

    bool numeric = std::all_of(actualSeed.begin(), actualSeed.end(), ::isdigit);
    if (numeric) {
      seeder->setSeed(static_cast<size_t>(std::stoull(actualSeed)));
    } else {
      seeder->setSeedString(actualSeed);
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
        if (data.dimensionMode == DimensionMode::BOUNDED || data.dimensionMode == DimensionMode::MAPPED) {
          ensureMapSize();
          float maxX = static_cast<float>(data.mapRect.w);
          float maxY = static_cast<float>(data.mapRect.h);
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

  void GameState::registerLuaFunctions(Project::Platform::Platform* platformPtr) {
    if (platformPtr) platform = platformPtr;
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
        record(Keys::LUA_CAMERA_ZOOM_IN);
        record(Keys::LUA_CAMERA_ZOOM_OUT);
        record(Keys::LUA_CAMERA_SHAKE);
        record(Keys::LUA_CAMERA_ROTATE);
        record(Keys::LUA_CHANGE_STATE);
        record(Keys::LUA_RESET_STATE);
        record(Keys::LUA_SET_BACKGROUND_COLOR);
        record(Keys::LUA_SET_BACKGROUND_IMAGE);
        record(Keys::LUA_SET_DARKNESS);
        record(Keys::LUA_SET_LAYER_DARKNESS);
        record(Keys::LUA_SET_DAY_LAPSE);
        record(Keys::LUA_SET_TIME_OF_DAY);
        record(Keys::LUA_SET_TIME_CYCLE_ACTIVE);
        record(Keys::LUA_SPAWN_ENTITY);
        record(Keys::LUA_START_ENTITY_SEEDER);
        record(Keys::LUA_ADD_ENTITY_TO_SEEDER);
        record(Keys::LUA_SET_PLAYER_ENTITY);
        record(Keys::LUA_SET_MAP_SIZE);
        record(Keys::LUA_LOAD_MAP_ASSET);
        record(Keys::LUA_EXIT_GAME);
        std::string serialized;
        for (const auto& f : funcs) serialized += f + "\n";
        persistentFunctionCache.setData(path, std::vector<char>(serialized.begin(), serialized.end()));
      }
      scriptFunctionCache[path] = funcs;
    } else {
      funcs = it->second;
    }
    
    auto ensure = [&](const std::string& name){
      if (std::find(funcs.begin(), funcs.end(), name) == funcs.end()) {
        funcs.push_back(name);
      }
    };
    ensure(Keys::LUA_CAMERA_ZOOM_IN);
    ensure(Keys::LUA_CAMERA_ZOOM_OUT);
    ensure(Keys::LUA_CAMERA_SHAKE);
    ensure(Keys::LUA_CAMERA_ROTATE);

    for (const auto& f : funcs) {
      if (f == Keys::LUA_SET_ACTIVE_CAMERA) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_setActiveCamera, this);
      } else if (f == Keys::LUA_CAMERA_ZOOM_IN) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_cameraZoomIn, this);
      } else if (f == Keys::LUA_CAMERA_ZOOM_OUT) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_cameraZoomOut, this);
      } else if (f == Keys::LUA_CAMERA_SHAKE) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_cameraShake, this);
      } else if (f == Keys::LUA_CAMERA_ROTATE) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_cameraRotate, this);
      } else if (f == Keys::LUA_CHANGE_STATE) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_changeState, this);
      } else if (f == Keys::LUA_RESET_STATE) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_resetState, this);
      } else if (f == Keys::LUA_SET_BACKGROUND_COLOR) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_setBackgroundColor, this);
      } else if (f == Keys::LUA_SET_BACKGROUND_IMAGE) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_setBackgroundImage, this);
      } else if (f == Keys::LUA_SET_DARKNESS) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_setDarkness, this);
      } else if (f == Keys::LUA_SET_LAYER_DARKNESS) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_setLayerDarkness, this);
      } else if (f == Keys::LUA_SET_DAY_LAPSE) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_setDayLapse, this);
      } else if (f == Keys::LUA_SET_TIME_OF_DAY) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_setTimeOfDay, this);
      } else if (f == Keys::LUA_SET_TIME_CYCLE_ACTIVE) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_setTimeCycleActive, this);
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
      } else if (f == Keys::LUA_LOAD_MAP_ASSET) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_loadMapAsset, this);
      } else if (f == Keys::LUA_EXIT_GAME && platform) {
        luaStateWrapper.registerFunction(f, LuaBindings::lua_exitGame, platform);
      }
    }
  }

  void GameState::clearScriptFunctionCache() {
    scriptFunctionCache.clear();
    persistentFunctionCache.load();
  }

  void GameState::registerEntityScriptOverride(const std::string& entityName, const std::string& scriptPath) {
    entityScriptOverrides[entityName] = scriptPath;
  }

  void GameState::applyEntityScriptOverride(Entity* entity) const {
    if (!entity) return;
    auto it = entityScriptOverrides.find(entity->getEntityName());
    if (it != entityScriptOverrides.end()) {
      entity->attachLuaScript(it->second);
    }
  }

  void GameState::removeEntityScriptOverride(const std::string& entityName) {
    entityScriptOverrides.erase(entityName);
  }

  void GameState::clearEntityScriptOverrides() {
    entityScriptOverrides.clear();
  }

  void GameState::setMapTiles(std::vector<Project::Handlers::BuiltTile>&& tiles, int x, int y, int width, int height) {
    if (data.dimensionMode == DimensionMode::MAPPED && !mapTiles.empty()) {
      getLogsManager().logError("MAPPED dimension allows only one map.");
      return;
    }

    if (data.dimensionMode == DimensionMode::MAPPED) {
      if (width > 0 && height > 0) {
        setMapSize(width, height);
      }
    }

    bool bounded = (data.dimensionMode == DimensionMode::BOUNDED && data.mapRect.w > 0 && data.mapRect.h > 0);
    const SDL_Rect& bounds = data.mapRect;

    std::vector<Project::Handlers::BuiltTile> cropped;
    cropped.reserve(tiles.size());

    for (auto& tile : tiles) {
      tile.dest.x += x;
      tile.dest.y += y;
      if (bounded) {
        int clipX = std::max(tile.dest.x, bounds.x);
        int clipY = std::max(tile.dest.y, bounds.y);
        int clipRight = std::min(tile.dest.x + tile.dest.w, bounds.x + bounds.w);
        int clipBottom = std::min(tile.dest.y + tile.dest.h, bounds.y + bounds.h);

        if (clipX >= clipRight || clipY >= clipBottom) {
          continue;
        }

        int offsetX = clipX - tile.dest.x;
        int offsetY = clipY - tile.dest.y;

        tile.dest.x = clipX;
        tile.dest.y = clipY;
        tile.dest.w = clipRight - clipX;
        tile.dest.h = clipBottom - clipY;

        tile.src.x += offsetX;
        tile.src.y += offsetY;
        tile.src.w = tile.dest.w;
        tile.src.h = tile.dest.h;
      }
      cropped.push_back(tile);
    }
    mapTiles.insert(mapTiles.end(), cropped.begin(), cropped.end());
  }

  void GameState::ensureMapSize() {
    if (data.dimensionMode != DimensionMode::BOUNDED && data.dimensionMode != DimensionMode::MAPPED) return;
    if (data.mapRect.w > 0 && data.mapRect.h > 0) return;

    data.mapRect.x = 0;
    data.mapRect.y = 0;
    if (renderer) {
      SDL_GetRendererOutputSize(renderer, &data.mapRect.w, &data.mapRect.h);
    }
    if (data.mapRect.w <= 0 || data.mapRect.h <= 0) {
      data.mapRect.w = Constants::DEFAULT_SCREEN_WIDTH;
      data.mapRect.h = Constants::DEFAULT_SCREEN_HEIGHT;
    }
  }

  void GameState::updateDayNightCycle(float deltaTime) {
    float cycleSeconds = data.dayLapseSeconds > 0.0f ? data.dayLapseSeconds : Constants::DEFAULT_DAY_LAPSE_SECONDS;
    float minutesPerSecond = Constants::DEFAULT_TOTAL_DAY_MINUTES / cycleSeconds;

    if (data.timeCycleActive) {
      data.timeOfDay += deltaTime * minutesPerSecond;
      if (data.timeOfDay >= Constants::DEFAULT_TOTAL_DAY_MINUTES) {
        data.timeOfDay -= Constants::DEFAULT_TOTAL_DAY_MINUTES;
      }
    }

    float minutes = data.timeOfDay;
    if (minutes < Constants::SUNRISE_START || minutes >= Constants::SUNSET_END) {
      data.darkness = Constants::DEFAULT_MAX_DARKNESS;
    } else if (minutes < Constants::DAY_START) {
      float t = (minutes - Constants::SUNRISE_START) / (Constants::DAY_START - Constants::SUNRISE_START);
      data.darkness = Constants::DEFAULT_MAX_DARKNESS * (Constants::DEFAULT_WHOLE - t);
    } else if (minutes < Constants::DAY_END) {
      data.darkness = Project::Libraries::Constants::ANGLE_0_DEG;
    } else {
      float t = (minutes - Constants::DAY_END) / (Constants::SUNSET_END - Constants::DAY_END);
      data.darkness = Constants::DEFAULT_MAX_DARKNESS * t;
    }
  }
}
