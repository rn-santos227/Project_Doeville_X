#include "LuaBindings.h"

#include "components/camera_component/CameraComponent.h"
#include "components/motion_component/MotionComponent.h"
#include "components/text_component/TextComponent.h"
#include "entities/EntitiesManager.h"
#include "factories/entity/EntitiesFactory.h"
#include "handlers/resources/ResourcesHandler.h"
#include "libraries/constants/Constants.h"
#include "libraries/categories/Categories.h"
#include "libraries/keys/Keys.h"
#include "states/GameState.h"
#include "states/GameStateManager.h"

namespace Project::Bindings::LuaBindings {
  using Project::States::GameState;
  using Project::States::GameStateManager;
  using Project::Entities::EntitiesManager;
  using Project::Factories::EntitiesFactory;
  using Project::Entities::Entity;

  namespace Components = Project::Libraries::Categories::Components;
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  int lua_addEntityToSeed(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_addEntityToSeed.");
    }

    const char* name = luaL_checkstring(L, 1);
    if (!name) {
      luaL_error(L, "Expected entity template name.");
      return 0;
    }

    state->addEntityToSeed(name);
    std::string sid;
    if (lua_gettop(L) >= 2 && lua_isstring(L, 2)) {
      sid = lua_tostring(L, 2);
    }

    state->addEntityToSeed(name, sid);
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

    std::unique_ptr<Entity> entity = state->getEntitiesFactory()->cloneEntity(name);
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
        state->getGlobalEntitiesManager()->addEntity(name, shared);
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
        mgr->addEntity(name, shared);
      } else {
        luaL_error(L, "No layers available in LayersManager.");
      }
    } else if (state->getEntitiesManager()) {
      state->getEntitiesManager()->addEntity(name, shared);
    } else {
      luaL_error(L, "EntitiesManager not set for this state.");
    }

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

    state->setPlayerEntity(name);
    return 0;
  }

  int lua_startEntitySeeder(lua_State* L) {
    GameState* state = static_cast<GameState*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!state) {
      return luaL_error(L, "Invalid GameState reference in lua_startEntitySeeder.");
    }

    int top = lua_gettop(L);
    std::string seed;
    std::string layer;
    std::string id;

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

    std::string seederId = state->startEntitySeeder(seed, layer, id);
    lua_pushstring(L, seederId.c_str());
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
}
