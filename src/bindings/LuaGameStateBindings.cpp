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

    const char* name = luaL_checkstring(L, 1);
    if (!name) {
      luaL_error(L, "Expected a state name string.");
      return 0;
    }

    state->getGameStateManager()->changeState(name);
    return 0;
  }

}
