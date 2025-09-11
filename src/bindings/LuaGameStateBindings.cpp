#include "LuaGameStateBindings.h"

#include <cmath>
#include <string>
#include <unordered_set>

#include <SDL.h>

#include "assets/AssetsManager.h"
#include "assets/map_asset/MapAsset.h"
#include "assets/tile_asset/TileAsset.h"
#include "components/ComponentType.h"
#include "components/camera_component/CameraComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "entities/ChunkSize.h"
#include "entities/EntitiesManager.h"
#include "factories/entity/EntitiesFactory.h"
#include "factories/asset/AssetsFactory.h"
#include "handlers/tile/TileHandler.h"
#include "libraries/constants/Constants.h"
#include "libraries/categories/Categories.h"
#include "libraries/keys/Keys.h"
#include "platform/Platform.h"
#include "states/GameState.h"
#include "states/GameStateManager.h"

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

  int lua_cameraShake(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    float duration = static_cast<float>(luaL_optnumber(L, Constants::INDEX_ONE, Constants::DEFAULT_CAMERA_SHAKE_DURATION));
    if (!state || !state->getActiveCamera()) {
      return 0;
    }
    state->getActiveCamera()->shake(duration);
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

  int lua_changeState(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_changeState.");
    }

    if (!state->getGameStateManager()) {
      return luaL_error(L, "GameStateManager not set for this state.");
    }

    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    if (!name) {
      luaL_error(L, "Expected a state name string.");
      return 0;
    }

    state->getGameStateManager()->changeState(name);
    return 0;
  }

  int lua_exitGame(lua_State* L) {
    auto* platform = static_cast<Project::Platform::Platform*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    if (!platform) {
      return luaL_error(L, "Invalid Platform reference in lua_exitGame.");
    }

    SDL_Event quitEvent{};
    quitEvent.type = SDL_QUIT;
    SDL_PushEvent(&quitEvent);
    platform->requestExit();
    return 0;
  }

  int lua_loadMapAsset(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_loadMapAsset.");
    }

    if (!assetsManagerPtr) {
      return luaL_error(L, "Assets system not initialized for lua_loadMapAsset.");
    }

    std::string assetId = luaL_checkstring(L, Constants::INDEX_ONE);
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

  int lua_resetState(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_resetState.");
    }

    state->reset();
    return 0;
  }

  int lua_setActiveCamera(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
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

  int lua_setBackgroundColor(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    Uint8 r = static_cast<Uint8>(luaL_checkinteger(L, Constants::INDEX_ONE));
    Uint8 g = static_cast<Uint8>(luaL_checkinteger(L, Constants::INDEX_TWO));
    Uint8 b = static_cast<Uint8>(luaL_checkinteger(L, Constants::INDEX_THREE));
    Uint8 a = static_cast<Uint8>(luaL_optinteger(L, Constants::INDEX_FOUR, Constants::FULL_ALPHA));
    state->setBackgroundColor(r, g, b, a);
    return 0; 
  }

  int lua_setBackgroundImage(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
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
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    float value = static_cast<float>(luaL_checknumber(L, Constants::INDEX_ONE));
    if (state) state->setDarkness(value);
    return 0;
  }

 int lua_setDayLapse(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    float seconds = static_cast<float>(luaL_checknumber(L, Constants::INDEX_ONE));
    if (state) state->setDayLapse(seconds);
    return 0;
  }

  int lua_setLayerDarkness(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    float value = static_cast<float>(luaL_checknumber(L, Constants::INDEX_TWO));
    if (state && name) state->setLayerDarkness(name, value);
    return 0;
  }

  int lua_setDayLapse(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    float seconds = static_cast<float>(luaL_checknumber(L, Constants::INDEX_ONE));
    if (state) state->setDayLapse(seconds);
    return 0;
  }

  int lua_setLayerDarkness(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    float value = static_cast<float>(luaL_checknumber(L, Constants::INDEX_TWO));
    if (state && name) state->setLayerDarkness(name, value);
    return 0;
  }

  int lua_setMapSize(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_setMapSize.");
    }

    int w = static_cast<int>(luaL_checkinteger(L, 1));
    int h = static_cast<int>(luaL_checkinteger(L, 2));
    state->setMapSize(w, h);
    return 0;
  }
}
