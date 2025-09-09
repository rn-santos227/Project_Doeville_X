#include "LuaBindings.h"

#include <cmath>
#include <string>
#include <unordered_set>

#include <SDL.h>

#include "assets/AssetsManager.h"
#include "assets/map_asset/MapAsset.h"
#include "assets/tile_asset/TileAsset.h"
#include "components/ComponentType.h"
#include "components/camera_component/CameraComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/meter_component/MeterComponent.h"
#include "components/motion_component/MotionComponent.h"
#include "components/spawner_component/SpawnerComponent.h"
#include "components/text_component/TextComponent.h"
#include "components/vision_component/VisionComponent.h"
#include "entities/ChunkSize.h"
#include "entities/EntitiesManager.h"
#include "factories/entity/EntitiesFactory.h"
#include "factories/asset/AssetsFactory.h"
#include "handlers/input/KeyActionResolver.h"
#include "handlers/resources/ResourcesHandler.h"
#include "handlers/tile/TileHandler.h"
#include "libraries/constants/Constants.h"
#include "libraries/categories/Categories.h"
#include "libraries/keys/Keys.h"
#include "platform/Platform.h"
#include "states/GameState.h"
#include "states/GameStateManager.h"
#include "utilities/physics/PhysicsUtils.h"

namespace Project::Bindings::LuaBindings {
  using Project::States::GameState;
  using Project::States::GameStateManager;
  using Project::Entities::EntitiesManager;
  using Project::Entities::Entity;
  using Project::Assets::AssetsManager;
  using Project::Assets::MapAsset;
  using Project::Assets::TileAsset;
  using Project::Factories::AssetsFactory;
  using Project::Factories::EntitiesFactory;
  using Project::Handlers::TileHandler;
  using Project::Platform::Platform;
  using Project::Utilities::PhysicsUtils;

  namespace Components = Project::Libraries::Categories::Components;
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  static AssetsManager* assetsManagerPtr = nullptr;
  static AssetsFactory* assetsFactoryPtr = nullptr;

  void setAssetsManager(AssetsManager* manager) { assetsManagerPtr = manager; }
  void setAssetsFactory(AssetsFactory* factory) { assetsFactoryPtr = factory; }

  int lua_addEntityToSeed(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_addEntityToSeed.");
    }

    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    if (!name) {
      luaL_error(L, "Expected entity template name.");
      return 0;
    }

    std::string sid;
    if (lua_gettop(L) >= Constants::INDEX_TWO && lua_isstring(L, Constants::INDEX_TWO)) {
      sid = lua_tostring(L, Constants::INDEX_TWO);
    }

    size_t count = Project::Libraries::Constants::INT_ONE;
    if (lua_gettop(L) >= Constants::INDEX_THREE && lua_isnumber(L, Constants::INDEX_THREE)) {
      count = static_cast<size_t>(lua_tonumber(L, Constants::INDEX_THREE));
    }

    state->addEntityToSeed(name, sid, count);
    return 0;
  }

  int lua_changeState(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_changeState.");
    }

    if (!state->getGameStateManager()) {
      return luaL_error(L, "GameStateManager not set for this state.");
    }

    const char* name = luaL_checkstring(L, 1);
    if (!name) {
      luaL_error(L, "Expected a state name string.");
      return 0;
    }

    state->getGameStateManager()->changeState(name);
    return 0;
  }

  int lua_destroyEntity(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    if (!manager || !name) {
      return 0;
    }

    if (manager->hasEntity(name)) {
      manager->removeEntity(name);
      return 0;
    }

    if (manager->getGameState()) {
      auto entity = manager->getGameState()->findEntity(name);
      if (entity && entity->getEntitiesManager()) {
        entity->getEntitiesManager()->removeEntity(name);
      }
    }

    return 0;
  }

  int lua_destroyEntities(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!manager || !lua_istable(L, 1)) {
      return 0;
    }

    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
      const char* name = lua_tostring(L, -1);
      if (name) {
        if (manager->hasEntity(name)) {
          manager->removeEntity(name);
        } else if (manager->getGameState()) {
          auto entity = manager->getGameState()->findEntity(name);
          if (entity && entity->getEntitiesManager()) {
            entity->getEntitiesManager()->removeEntity(name);
          }
        }
      }
      lua_pop(L, 1);
    }
    return 0;
  }

  int lua_destroySelf(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!manager) {
      return 0;
    }

    lua_getglobal(L, Keys::ID);
    const char* name = lua_isstring(L, -1) ? lua_tostring(L, -1) : nullptr;
    lua_pop(L, 1);
    if (!name) {
      return 0;
    }

    if (manager->hasEntity(name)) {
      manager->removeEntity(name);
      return 0;
    }

    if (manager->getGameState()) {
      auto entity = manager->getGameState()->findEntity(name);
      if (entity && entity->getEntitiesManager()) {
        entity->getEntitiesManager()->removeEntity(name);
      }
    }

    return 0;
  }

  int lua_spawnEntity(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_spawnEntity.");
    }

    const char* name = luaL_checkstring(L, 1);
    if (!name) {
      luaL_error(L, "Expected an entity name string.");
      return 0;
    }

    bool hasX = lua_gettop(L) >= Constants::INDEX_TWO && lua_isnumber(L, Constants::INDEX_TWO);
    bool hasY = lua_gettop(L) >= Constants::INDEX_THREE && lua_isnumber(L, Constants::INDEX_THREE);
    bool hasLayer = lua_gettop(L) >= Constants::INDEX_FOUR && lua_isstring(L, Constants::INDEX_FOUR);
    float posX = 0.0f;
    float posY = 0.0f;
    if (hasX) posX = static_cast<float>(lua_tonumber(L, Constants::INDEX_TWO));
    if (hasY) posY = static_cast<float>(lua_tonumber(L, Constants::INDEX_THREE));
    std::string targetLayer;
    if (hasLayer) targetLayer = lua_tostring(L, Constants::INDEX_FOUR);

    if (!state->getEntitiesFactory()) {
      luaL_error(L, "EntitiesFactory not set for this state.");
      return 0;
    }

    bool isGlobal = state->getEntitiesFactory()->isEntityGlobal(name);
    if (isGlobal && state->getGlobalEntitiesManager() && state->getGlobalEntitiesManager()->hasEntity(name)) {
      return 0;
    }

    EntitiesFactory::EntityPtr entity = state->getEntitiesFactory()->cloneEntity(name);
    if (!entity) {
      luaL_error(L, ("Failed to clone entity: " + std::string(name)).c_str());
      return 0;
    }
  
    if (hasX) entity->getLuaStateWrapper().setGlobalNumber(Keys::X, posX);
    if (hasY) entity->getLuaStateWrapper().setGlobalNumber(Keys::Y, posY);

    entity->initialize();
    std::shared_ptr<Entity> shared = std::move(entity);

    if (isGlobal) {
      if (state->getGlobalEntitiesManager()) {
        state->getGlobalEntitiesManager()->addEntity(shared, name);
      } else {
        luaL_error(L, "Global EntitiesManager not set for this state.");
      }
    } else if (state->getLayersManager()) {
      std::shared_ptr<EntitiesManager> mgr;
      if (!targetLayer.empty()) {
        mgr = state->getLayersManager()->getLayer(targetLayer);
      } else {
        mgr = state->getLayersManager()->getFirstLayer();     
      }

      if (mgr) {
        mgr->addEntity(shared, name);
      } else {
        luaL_error(L, "No layers available in LayersManager.");
      }
    } else if (state->getEntitiesManager()) {
      state->getEntitiesManager()->addEntity(shared, name);
    } else {
      luaL_error(L, "EntitiesManager not set for this state.");
    }

    return 0;
  }
  
  int lua_isActionPressed(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    const char* action = luaL_checkstring(L, 2);
    if (!manager || !name || !action) {
      lua_pushboolean(L, 0);
      return 1;
    }

    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }
    if (!entity) {
      lua_pushboolean(L, 0);
      return 1;
    }

    auto* keys = dynamic_cast<Project::Components::KeysComponent*>(entity->getComponent(Components::KEYS_COMPONENT));
    if (!keys) {
      lua_pushboolean(L, 0);
      return 1;
    }

    auto act = Project::Handlers::KeyActionResolver::resolve(action);
    if (act == Project::Handlers::KeyAction::NONE) {
      lua_pushboolean(L, 0);
      return 1;
    }

    lua_pushboolean(L, keys->isActionTriggered(act));
    return 1;
  }

  int lua_ignoreCollisionsWith(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    if (!manager || !name || !lua_istable(L, 2)) {
      return 0;
    }

    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }
    if (!entity) return 0;

    auto* box = dynamic_cast<Project::Components::BoundingBoxComponent*>(entity->getComponent(Components::BOUNDING_BOX_COMPONENT));
    if (!box) return 0;

    std::vector<std::string> targets;
    lua_pushnil(L);
    while (lua_next(L, Constants::INDEX_TWO) != 0) {
      if (lua_isstring(L, -1)) {
        targets.emplace_back(lua_tostring(L, -1));
      }
      lua_pop(L, 1);
    }

    box->setIgnoredEntities(targets);
    return 0;
  }

  int lua_resetState(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_resetState.");
    }

    state->reset();
    return 0;
  }

  int lua_exitGame(lua_State* L) {
    auto* platform = static_cast<Project::Platform::Platform*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!platform) {
      return luaL_error(L, "Invalid Platform reference in lua_exitGame.");
    }

    SDL_Event quitEvent{};
    quitEvent.type = SDL_QUIT;
    SDL_PushEvent(&quitEvent);
    platform->requestExit();
    return 0;
  }

  int lua_setActiveCamera(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    if (!state || !name) {
      return luaL_error(L, "Invalid parameters for setActiveCamera");
    }

    auto entity = state->findEntity(name);
    if (!entity) {
      state->getLogsManager().logError(std::string("Camera entity not found: ") + name);
      return 0;
    }

    auto* cam = dynamic_cast<Project::Components::CameraComponent*>(entity->getComponent(Components::CAMERA_COMPONENT));
    if (!cam) {
       state->getLogsManager().logError(std::string("Entity has no CameraComponent: ") + name);
       return 0;
    }

    state->setActiveCamera(cam);
    return 0;
  }

  int lua_cameraZoomIn(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    float amount = static_cast<float>(luaL_optnumber(L, Constants::INDEX_ONE, Constants::DEFAULT_CAMERA_ZOOM_SPEED));
    auto* cam = state ? state->getActiveCamera() : nullptr;
    auto* handler = Project::Components::GraphicsComponent::getCameraHandler();
    if (cam) {
      cam->zoomIn(amount);
    } else if (handler) {
      handler->zoomIn(amount);
    }
    return 0;
  }

  int lua_cameraZoomOut(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    float amount = static_cast<float>(luaL_optnumber(L, Constants::INDEX_ONE, Constants::DEFAULT_CAMERA_ZOOM_SPEED));
    auto* cam = state ? state->getActiveCamera() : nullptr;
    auto* handler = Project::Components::GraphicsComponent::getCameraHandler();
    if (cam) {
      cam->zoomOut(amount);
    } else if (handler) {
      handler->zoomOut(amount);
    }
    return 0;
  }

  int lua_cameraRotate(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    float amount = static_cast<float>(luaL_optnumber(L, Constants::INDEX_ONE, Constants::DEFAULT_CAMERA_ROTATION_STEP));
    auto* cam = state ? state->getActiveCamera() : nullptr;
    auto* handler = Project::Components::GraphicsComponent::getCameraHandler();
    if (cam) {
      cam->rotate(amount);
    } else if (handler) {
      handler->rotate(amount);
    }
    return 0;
  }

  int lua_cameraShake(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    float duration = static_cast<float>(luaL_optnumber(L, Constants::INDEX_ONE, Constants::DEFAULT_CAMERA_SHAKE_DURATION));
    if (!state || !state->getActiveCamera()) {
      return 0;
    }
    state->getActiveCamera()->shake(duration);
    return 0;
  }

  int lua_setBackgroundColor(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    Uint8 r = static_cast<Uint8>(luaL_checkinteger(L, Constants::INDEX_ONE));
    Uint8 g = static_cast<Uint8>(luaL_checkinteger(L, Constants::INDEX_TWO));
    Uint8 b = static_cast<Uint8>(luaL_checkinteger(L, Constants::INDEX_THREE));
    Uint8 a = static_cast<Uint8>(luaL_optinteger(L, Constants::INDEX_FOUR, Constants::FULL_ALPHA));
    state->setBackgroundColor(r, g, b, a);
    return 0; 
  }

  int lua_setBackgroundImage(lua_State* L) {
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

  int lua_setDarkness(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    float value = static_cast<float>(luaL_checknumber(L, Constants::INDEX_ONE));
    if (state) state->setDarkness(value);
    return 0;
  }

  int lua_setLayerDarkness(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    float value = static_cast<float>(luaL_checknumber(L, Constants::INDEX_TWO));
    if (state && name) state->setLayerDarkness(name, value);
    return 0;
  }

  int lua_setTimeDarknessLayer(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_optstring(L, Constants::INDEX_ONE, Keys::STATE);
    if (state) state->setTimeDarknessLayer(name ? name : Constants::EMPTY_STRING);
    return 0;
  }

  int lua_setDayLapse(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    float seconds = static_cast<float>(luaL_checknumber(L, Constants::INDEX_ONE));
    if (state) state->setDayLapse(seconds);
    return 0;
  }

  int lua_setTimeOfDay(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    float minutes = static_cast<float>(luaL_checknumber(L, Constants::INDEX_ONE));
    if (state) state->setTimeOfDay(minutes);
    return 0;
  }

  int lua_setTimeCycleActive(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    bool active = lua_toboolean(L, Constants::INDEX_ONE);
    if (state) state->setTimeCycleActive(active);
    return 0;
  }

  int lua_setPlayerEntity(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_setPlayerEntity.");
    }

    const char* name = luaL_checkstring(L, 1);
    if (!name) {
      luaL_error(L, "Expected entity name.");
      return 0;
    }

    bool hasX = lua_gettop(L) >= Constants::INDEX_TWO && lua_isnumber(L, Constants::INDEX_TWO);
    bool hasY = lua_gettop(L) >= Constants::INDEX_THREE && lua_isnumber(L, Constants::INDEX_THREE);
    float x = 0.0f;
    float y = 0.0f;
    if (hasX) x = static_cast<float>(lua_tonumber(L, Constants::INDEX_TWO));
    if (hasY) y = static_cast<float>(lua_tonumber(L, Constants::INDEX_THREE));
    state->setPlayerEntity(name, x, y, hasX && hasY);
    return 0;
  }

  int lua_setMapSize(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_setMapSize.");
    }

    int w = static_cast<int>(luaL_checkinteger(L, 1));
    int h = static_cast<int>(luaL_checkinteger(L, 2));
    state->setMapSize(w, h);
    return 0;
  }

  int lua_startEntitySeeder(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_startEntitySeeder.");
    }

    int top = lua_gettop(L);
    std::string seed;
    std::string layer;
    std::string id;
    Project::Entities::ChunkSize chunkSize;
    

    if (top >= Constants::INDEX_ONE && !lua_isnil(L, Constants::INDEX_ONE)) {
      if (lua_isnumber(L, Constants::INDEX_ONE)) {
        seed = std::to_string(static_cast<size_t>(lua_tonumber(L, Constants::INDEX_ONE)));
      } else if (lua_isstring(L, Constants::INDEX_ONE)) {
        seed = lua_tostring(L, Constants::INDEX_ONE);
      }
    }
    if (top >= Constants::INDEX_TWO && lua_isstring(L, Constants::INDEX_TWO)) {
      layer = lua_tostring(L, Constants::INDEX_TWO);
    }
    if (top >= Constants::INDEX_THREE && lua_isstring(L, Constants::INDEX_THREE)) {
      id = lua_tostring(L, Constants::INDEX_THREE);
    }

    if (top >= Constants::INDEX_FOUR && !lua_isnil(L, Constants::INDEX_FOUR)) {
      if (lua_istable(L, Constants::INDEX_FOUR)) {
        lua_getfield(L, Constants::INDEX_FOUR, Keys::X);
        if (lua_isnumber(L, -1)) chunkSize.x = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);
        lua_getfield(L, Constants::INDEX_FOUR, Keys::Y);
        if (lua_isnumber(L, -1)) chunkSize.y = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);
        lua_getfield(L, Constants::INDEX_FOUR, Keys::Z);
        if (lua_isnumber(L, -1)) chunkSize.z = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);
      } else if (lua_isnumber(L, Constants::INDEX_FOUR)) {
        float val = static_cast<float>(lua_tonumber(L, Constants::INDEX_FOUR));
        chunkSize.x = val;
        chunkSize.y = val;
      }
    }

    std::string seederId = state->startEntitySeeder(seed, layer, id, chunkSize);
    lua_pushstring(L, seederId.c_str());
    return 1;
  }

  int lua_loadMapAsset(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_loadMapAsset.");
    }

    if (!assetsManagerPtr) {
      return luaL_error(L, "Assets system not initialized for lua_loadMapAsset.");
    }

    std::string assetId = luaL_checkstring(L, 1);
    MapAsset* mapAsset = assetsManagerPtr->getMap(assetId);
    if (!mapAsset) {
      std::string error = "Map asset not found: " + assetId;
      return luaL_error(L, error.c_str());
    }

    int tileW = Constants::DEFAULT_COMPONENT_SIZE;
    int tileH = Constants::DEFAULT_COMPONENT_SIZE;
    const auto& mappings = mapAsset->getTileMappings();
    if (!mappings.empty()) {
      auto tileAssetName = mappings.begin()->second;
      TileAsset* tile = assetsManagerPtr->getTile(tileAssetName);
      if (tile) {
        tileW = tile->getTileWidth();
        tileH = tile->getTileHeight();
      }
    }

    int top = lua_gettop(L);
    int x = 0;
    int y = 0;
    if (top >= Constants::INDEX_TWO && lua_isnumber(L, Constants::INDEX_TWO)) x = static_cast<int>(lua_tointeger(L, Constants::INDEX_TWO));
    if (top >= Constants::INDEX_THREE && lua_isnumber(L, Constants::INDEX_THREE)) y = static_cast<int>(lua_tointeger(L, Constants::INDEX_THREE));

    int mapW = mapAsset->getWidth() * tileW;
    int mapH = mapAsset->getHeight() * tileH;
    if ((state->getMapRect().w == 0 && state->getMapRect().h == 0) ||
        state->getDimensionMode() == Project::States::DimensionMode::MAPPED) {
      state->setMapSize(mapW, mapH);
    }

    TileHandler builder(state->getRenderer(), state->getLogsManager(), *assetsManagerPtr);
    auto tiles = builder.buildMap(assetId);
    state->setMapTiles(std::move(tiles), x, y, mapW, mapH);

    return 0;
  }

  int lua_getEntityDetails(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    if (!manager || !name) {
      lua_pushnil(L);
      return 1;
    }

    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      return 1;
    }

    lua_newtable(L);
    lua_pushstring(L, entity->getEntityID().c_str());
    lua_setfield(L, -2, Keys::ID);
    lua_pushstring(L, entity->getEntityName().c_str());
    lua_setfield(L, -2, Keys::NAME);
    lua_pushstring(L, entity->getGroup().c_str());
    lua_setfield(L, -2, Keys::GROUP);
    lua_pushnumber(L, entity->getX());
    lua_setfield(L, -2, Keys::X);
    lua_pushnumber(L, entity->getY());
    lua_setfield(L, -2, Keys::Y);
    lua_pushnumber(L, entity->getZ());
    lua_setfield(L, -2, Keys::Z);

    return 1;
  }

  int lua_getEntitySpeed(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    if (!manager || !name) {
      lua_pushnil(L);
      return 1;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      return 1;

    }
    auto* motion = dynamic_cast<Project::Components::MotionComponent*>(entity->getComponent(Components::MOTION_COMPONENT));
    if (!motion) {
      lua_pushnil(L);
      return 1;
    }
    lua_pushnumber(L, motion->getCurrentSpeed());
    return 1;
  }

  int lua_getEntityVelocity(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    if (!manager || !name) {
      lua_pushnil(L);
      lua_pushnil(L);
      return Constants::INDEX_TWO;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      lua_pushnil(L);
      return Constants::INDEX_TWO;
    }

    auto* motion = dynamic_cast<Project::Components::MotionComponent*>(entity->getComponent(Components::MOTION_COMPONENT));
    if (!motion) {
      lua_pushnil(L);
      lua_pushnil(L);
      return Constants::INDEX_TWO;
    }

    lua_pushnumber(L, motion->getVelocityX());
    lua_pushnumber(L, motion->getVelocityY());
    return Constants::INDEX_TWO;
  }

  int lua_getEntityRotation(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    if (!manager || !name) {
      lua_pushnil(L);
      return 1;
    }

    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      return 1;
    }

    float rotation = 0.0f;
    bool found = false;
    if (auto* physics = dynamic_cast<Project::Components::PhysicsComponent*>(entity->getComponent(Components::PHYSICS_COMPONENT))) {
      rotation = physics->getRotation();
      found = true;
    } else if (auto* box = dynamic_cast<Project::Components::BoundingBoxComponent*>(entity->getComponent(Components::BOUNDING_BOX_COMPONENT))) {
      rotation = box->getRotation();
      found = true;
    } else if (auto* gfx = dynamic_cast<Project::Components::GraphicsComponent*>(entity->getComponent(Components::GRAPHICS_COMPONENT))) {
      rotation = gfx->getRotation();
      found = true;
    }

    if (found) {
      lua_pushnumber(L, rotation);
    } else {
      lua_pushnil(L);
    }
    return 1;
  }

  int lua_getNetworkPayload(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    if (!manager || !name) {
      lua_pushnil(L);
      return 1;
    }

    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      return 1;
    }

    auto* net = dynamic_cast<Project::Components::NetworkComponent*>(
        entity->getComponent(Components::NETWORK_COMPONENT));
    if (!net) {
      lua_pushnil(L);
      return 1;
    }

    const std::string& payload = net->getLastPayload();
    if (payload.empty()) {
      lua_pushnil(L);
    } else {
      lua_pushlstring(L, payload.c_str(), payload.size());
    }
    return 1;
  }

  int lua_getVisibleEntities(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    if (!manager || !name) {
      lua_pushnil(L);
      return 1;
    }

    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      return 1;
    }
    
    auto components = entity->getComponentsByType(Project::Components::ComponentType::VISION);
    std::unordered_set<const Entity*> unique;
    for (auto* base : components) {
      if (auto* vision = dynamic_cast<Project::Components::VisionComponent*>(base)) {
        const auto& targets = vision->getVisibleEntities();
        for (const auto* target : targets) {
          unique.insert(target);
        }
      }
    }

    if (unique.empty()) {
      lua_pushnil(L);
      return 1;
    }

    lua_newtable(L);
    int index = Constants::INDEX_ONE;
    for (const auto* target : unique) {
      lua_pushstring(L, target->getEntityID().c_str());
      lua_rawseti(L, -2, index++);
    }

    return 1;
  }

  int lua_setEntityText(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    const char* text = luaL_checkstring(L, 2);
    if (!manager || !name || !text) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) return 0;
    auto* textComp = dynamic_cast<Project::Components::TextComponent*>(entity->getComponent(Components::TEXT_COMPONENT));
    if (!textComp) return 0;
    textComp->setText(text);
    return 0;
  }

  int lua_setTimerActive(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    int active = lua_toboolean(L, 2);
    if (!manager || !name) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) return 0;
    auto* timer = dynamic_cast<Project::Components::TimerComponent*>(entity->getComponent(Components::TIMER_COMPONENT));
    if (!timer) return 0;

    timer->setActive(active != 0);
    if (active) {
      timer->reset();
    }
    return 0;
  }

  int lua_setMeterActive(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    int active = lua_toboolean(L, 2);
    if (!manager || !name) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) return 0;
    auto* meter = dynamic_cast<Project::Components::MeterComponent*>(entity->getComponent(Components::METER_COMPONENT));
    if (!meter) return 0;

    meter->setActive(active != 0);
    return 0;
  }

  int lua_setColor(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    if (!manager || !name) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) return 0;
    auto* gfx = dynamic_cast<Project::Components::GraphicsComponent*>(entity->getComponent(Components::GRAPHICS_COMPONENT));
    if (!gfx) return 0;

    int r = static_cast<int>(luaL_checkinteger(L, Constants::INDEX_TWO));
    int g = static_cast<int>(luaL_checkinteger(L, Constants::INDEX_THREE));
    int b = static_cast<int>(luaL_checkinteger(L, Constants::INDEX_FOUR));
    int a = Constants::FULL_ALPHA;
    if (lua_gettop(L) >= Constants::INDEX_FIVE && lua_isnumber(L, Constants::INDEX_FIVE)) {
      a = static_cast<int>(luaL_checkinteger(L, Constants::INDEX_FIVE));
    }

    SDL_Color color{static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a)};
    gfx->setColor(color);
    return 0;
  }

  int lua_addNumericValue(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    const char* key = luaL_checkstring(L, Constants::INDEX_TWO);
    if (!manager || !name || !key) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) return 0;
    auto* numeric = dynamic_cast<Project::Components::NumericComponent*>(entity->getComponent(Components::NUMERIC_COMPONENT));
    if (!numeric) return 0;

    float amount = static_cast<float>(luaL_checknumber(L, Constants::INDEX_THREE));
    numeric->add(key, amount);
    return 0;
  }

  int lua_subtractNumericValue(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    const char* key = luaL_checkstring(L, Constants::INDEX_TWO);
    if (!manager || !name || !key) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) return 0;
    auto* numeric = dynamic_cast<Project::Components::NumericComponent*>(entity->getComponent(Components::NUMERIC_COMPONENT));
    if (!numeric) return 0;

    float amount = static_cast<float>(luaL_checknumber(L, Constants::INDEX_THREE));
    numeric->subtract(key, amount);
    return 0;
  }

  int lua_multiplyNumericValue(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    const char* key = luaL_checkstring(L, Constants::INDEX_TWO);
    if (!manager || !name || !key) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) return 0;
    auto* numeric = dynamic_cast<Project::Components::NumericComponent*>(entity->getComponent(Components::NUMERIC_COMPONENT));
    if (!numeric) return 0;

    float amount = static_cast<float>(luaL_checknumber(L, Constants::INDEX_THREE));
    numeric->multiply(key, amount);
    return 0;
  }

  int lua_divideNumericValue(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    const char* key = luaL_checkstring(L, Constants::INDEX_TWO);
    if (!manager || !name || !key) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) return 0;
    auto* numeric = dynamic_cast<Project::Components::NumericComponent*>(entity->getComponent(Components::NUMERIC_COMPONENT));
    if (!numeric) return 0;

    float amount = static_cast<float>(luaL_checknumber(L, Constants::INDEX_THREE));
    numeric->divide(key, amount);
    return 0;
  }

  int lua_setNumericValue(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    const char* key = luaL_checkstring(L, 2);
    if (!manager || !name || !key) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) return 0;
    auto* numeric = dynamic_cast<Project::Components::NumericComponent*>(entity->getComponent(Components::NUMERIC_COMPONENT));
    if (!numeric) return 0;

    float value = static_cast<float>(luaL_checknumber(L, 3));
    float limit = 0.0f;
    if (lua_gettop(L) >= 4 && lua_isnumber(L, 4)) {
      limit = static_cast<float>(lua_tonumber(L, 4));
      numeric->setLimit(key, limit);
    }
    numeric->setValue(key, value);
    return 0;
  }

  int lua_getNumericValue(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    const char* key = luaL_checkstring(L, 2);
    if (!manager || !name || !key) {
      lua_pushnil(L);
      return 1;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      return 1;
    }
    auto* numeric = dynamic_cast<Project::Components::NumericComponent*>(entity->getComponent(Components::NUMERIC_COMPONENT));
    if (!numeric || !numeric->has(key)) {
      lua_pushnil(L);
      return 1;
    }

    lua_pushnumber(L, numeric->getValue(key));
    return 1;
  }

  int lua_stopTimer(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    if (!manager || !name) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) return 0;
    auto* timer = dynamic_cast<Project::Components::TimerComponent*>(entity->getComponent(Components::TIMER_COMPONENT));
    if (!timer) return 0;

    timer->stop();
    return 0;
  }

  int lua_brakeEntity(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    if (!manager || !name) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) return 0;
    auto* motion = dynamic_cast<Project::Components::MotionComponent*>(entity->getComponent(Components::MOTION_COMPONENT));
    if (!motion) return 0;

    motion->brake();
    return 0;
  }
  
  int lua_turnLeft(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    float speed = static_cast<float>(luaL_optnumber(L, 2, Constants::DEFAULT_ROTATION_SPEED));
    if (!manager || !name) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }
    if (!entity) return 0;
    auto* motion = dynamic_cast<Project::Components::MotionComponent*>(entity->getComponent(Components::MOTION_COMPONENT));
    if (!motion) return 0;
    motion->turn(speed, true);
    return 0;
  }

  int lua_turnRight(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    float speed = static_cast<float>(luaL_optnumber(L, 2, Constants::DEFAULT_ROTATION_SPEED));
    if (!manager || !name) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }
    if (!entity) return 0;
    auto* motion = dynamic_cast<Project::Components::MotionComponent*>(entity->getComponent(Components::MOTION_COMPONENT));
    if (!motion) return 0;
    motion->turn(speed, false);
    return 0;
  }

  int lua_getCollidedEntity(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    if (!manager || !name) {
      lua_pushnil(L);
      return 1;
    }

    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }
    if (!entity) {
      lua_pushnil(L);
      return 1;
    }

    auto* myBox = dynamic_cast<Project::Components::BoundingBoxComponent*>(entity->getComponent(Components::BOUNDING_BOX_COMPONENT));
    if (!myBox) {
      lua_pushnil(L);
      return 1;
    }

    std::vector<std::string> targets;
    if (lua_gettop(L) >= 2 && lua_istable(L, 2)) {
      lua_pushnil(L);
      while (lua_next(L, 2) != 0) {
        if (lua_isstring(L, -1)) {
          targets.emplace_back(lua_tostring(L, -1));
        }
        lua_pop(L, 1);
      }
    }

    const auto& myRects = myBox->getBoxes();
    const auto& myCircles = myBox->getCircles();
    const auto& myOBB = myBox->getOrientedBoxes();
    const bool myRot = myBox->isRotationEnabled();

    for (const auto& [id, other] : manager->getAllEntities()) {
      auto otherEntity = other.get();
      if (!otherEntity || otherEntity == entity.get()) continue;
      if (!targets.empty()) {
        bool match = false;
        for (const auto& t : targets) {
          if (otherEntity->getEntityName() == t) { match = true; break; }
        }
        if (!match) continue;
      }
      auto* otherBox = dynamic_cast<Project::Components::BoundingBoxComponent*>(otherEntity->getComponent(Components::BOUNDING_BOX_COMPONENT));
      if (!otherBox) continue;

      const auto& otherRects = otherBox->getBoxes();
      const auto& otherCircles = otherBox->getCircles();
      const auto& otherOBB = otherBox->getOrientedBoxes();
      const bool otherRot = otherBox->isRotationEnabled();

      // Box vs Box
      for (size_t i = 0; i < myRects.size(); ++i) {
        for (size_t j = 0; j < otherRects.size(); ++j) {
          bool collides = false;
          if (myRot || otherRot) {
            if (i < myOBB.size() && j < otherOBB.size()) {
              collides = Project::Utilities::PhysicsUtils::checkCollision(myOBB[i], otherOBB[j]);
            }
          } else {
            collides = Project::Utilities::PhysicsUtils::checkCollision(myRects[i], otherRects[j]);
          }
          if (collides) {
            lua_pushstring(L, otherEntity->getEntityID().c_str());
            return 1;
          }
        }
      }

      // Box vs Circle
      for (const auto& rect : myRects) {
        for (const auto& circle : otherCircles) {
          if (Project::Utilities::PhysicsUtils::checkCollision(rect, circle)) {
            lua_pushstring(L, otherEntity->getEntityID().c_str());
            return 1;
          }
        }
      }

      // Circle vs Circle
      for (const auto& c1 : myCircles) {
        for (const auto& c2 : otherCircles) {
          if (Project::Utilities::PhysicsUtils::checkCollision(c1, c2)) {
            lua_pushstring(L, otherEntity->getEntityID().c_str());
            return 1;
          }
        }
      }

      // Circle vs Box
      for (const auto& circle : myCircles) {
        for (const auto& rect : otherRects) {
          if (Project::Utilities::PhysicsUtils::checkCollision(rect, circle)) {
            lua_pushstring(L, otherEntity->getEntityID().c_str());
            return 1;
          }
        }
      }
    }

    lua_pushnil(L);
    return 1;
  }
  
  int lua_factoryChangeState(lua_State* L) {
    auto* manager = static_cast<GameStateManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!manager) {
      return luaL_error(L, "Invalid GameStateManager reference in lua_changeState.");
    }

    const char* name = luaL_checkstring(L, 1);
    if (!name) {
      luaL_error(L, "Expected a state name string.");
      return 0;
    }

    manager->changeState(name);
    return 0;
  }

  int lua_spawn(lua_State* L) {
    auto* comp = static_cast<Project::Components::SpawnerComponent*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!comp) return 0;

    float offX = 0.0f;
    float offY = 0.0f;
    float velX = 0.0f;
    float velY = 0.0f;

    if (lua_gettop(L) >= Constants::INDEX_ONE && lua_isnumber(L, Constants::INDEX_ONE)) {
      offX = static_cast<float>(lua_tonumber(L, Constants::INDEX_ONE));
    }
    
    if (lua_gettop(L) >= Constants::INDEX_TWO && lua_isnumber(L, Constants::INDEX_TWO)) {
      offY = static_cast<float>(lua_tonumber(L, Constants::INDEX_TWO));
    }
    
    if (lua_gettop(L) >= Constants::INDEX_THREE && lua_isnumber(L, Constants::INDEX_THREE)) {
      velX = static_cast<float>(lua_tonumber(L, Constants::INDEX_THREE));
    }
    
    if (lua_gettop(L) >= Constants::INDEX_FOUR && lua_isnumber(L, Constants::INDEX_FOUR)) {
      velY = static_cast<float>(lua_tonumber(L, Constants::INDEX_FOUR));
    }

    comp->spawn(offX, offY, velX, velY);
    return 0;
  }
}
