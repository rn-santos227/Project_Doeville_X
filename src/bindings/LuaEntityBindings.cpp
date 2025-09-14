#include "LuaEntityBindings.h"

#include <cmath>
#include <string>
#include <unordered_set>

#include "components/ComponentType.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/meter_component/MeterComponent.h"
#include "components/motion_component/MotionComponent.h"
#include "components/spawner_component/SpawnerComponent.h"
#include "components/text_component/TextComponent.h"
#include "components/vision_component/VisionComponent.h"
#include "components/numeric_component/NumericComponent.h"
#include "components/timer_component/TimerComponent.h"
#include "components/keys_component/KeysComponent.h"
#include "components/network_component/NetworkComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "entities/EntitiesManager.h"
#include "handlers/input/KeyActionResolver.h"
#include "handlers/resources/ResourcesHandler.h"
#include "libraries/constants/Constants.h"
#include "libraries/categories/Categories.h"
#include "libraries/keys/Keys.h"
#include "services/network/NetworkProtocolResolver.h"
#include "states/GameState.h"
#include "states/GameStateManager.h"
#include "utilities/physics/PhysicsUtils.h"

namespace Project::Bindings::LuaBindings {
  using Project::States::GameState;
  using Project::States::GameStateManager;
  using Project::Entities::EntitiesManager;
  using Project::Entities::Entity;
  using Project::Utilities::PhysicsUtils;

  namespace Components = Project::Libraries::Categories::Components;
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  int lua_getCollidedEntity(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    if (!manager || !name) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }

    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }
    if (!entity) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }

    auto* myBox = dynamic_cast<Project::Components::BoundingBoxComponent*>(entity->getComponent(Components::BOUNDING_BOX_COMPONENT));
    if (!myBox) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }

    std::vector<std::string> targets;
    if (lua_gettop(L) >= Constants::INDEX_TWO && lua_istable(L, Constants::INDEX_TWO)) {
      lua_pushnil(L);
      while (lua_next(L, Constants::INDEX_TWO) != 0) {
        if (lua_isstring(L, -Constants::INDEX_ONE)) {
          targets.emplace_back(lua_tostring(L, -Constants::INDEX_ONE));
        }
        lua_pop(L, Constants::INDEX_ONE);
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
            return Constants::INDEX_ONE;
          }
        }
      }

      // Box vs Circle
      for (const auto& rect : myRects) {
        for (const auto& circle : otherCircles) {
          if (Project::Utilities::PhysicsUtils::checkCollision(rect, circle)) {
            lua_pushstring(L, otherEntity->getEntityID().c_str());
            return Constants::INDEX_ONE;
          }
        }
      }

      // Circle vs Circle
      for (const auto& c1 : myCircles) {
        for (const auto& c2 : otherCircles) {
          if (Project::Utilities::PhysicsUtils::checkCollision(c1, c2)) {
            lua_pushstring(L, otherEntity->getEntityID().c_str());
            return Constants::INDEX_ONE;
          }
        }
      }

      // Circle vs Box
      for (const auto& circle : myCircles) {
        for (const auto& rect : otherRects) {
          if (Project::Utilities::PhysicsUtils::checkCollision(rect, circle)) {
            lua_pushstring(L, otherEntity->getEntityID().c_str());
            return Constants::INDEX_ONE;
          }
        }
      }
    }

    lua_pushnil(L);
    return Constants::INDEX_ONE;
  }

  int lua_getEntityDetails(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    if (!manager || !name) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }

    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }

    lua_newtable(L);
    lua_pushstring(L, entity->getEntityID().c_str());
    lua_setfield(L, -Constants::INDEX_TWO, Keys::ID);
    lua_pushstring(L, entity->getEntityName().c_str());
    lua_setfield(L, -Constants::INDEX_TWO, Keys::NAME);
    lua_pushstring(L, entity->getGroup().c_str());
    lua_setfield(L, -Constants::INDEX_TWO, Keys::GROUP);
    lua_pushnumber(L, entity->getX());
    lua_setfield(L, -Constants::INDEX_TWO, Keys::X);
    lua_pushnumber(L, entity->getY());
    lua_setfield(L, -Constants::INDEX_TWO, Keys::Y);
    lua_pushnumber(L, entity->getZ());
    lua_setfield(L, -Constants::INDEX_TWO, Keys::Z);

    return Constants::INDEX_ONE;
  }

  int lua_getEntitySpeed(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    if (!manager || !name) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;

    }
    auto* motion = dynamic_cast<Project::Components::MotionComponent*>(entity->getComponent(Components::MOTION_COMPONENT));
    if (!motion) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }
    lua_pushnumber(L, motion->getCurrentSpeed());
    return Constants::INDEX_ONE;
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
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, 1);
    if (!manager || !name) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }

    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
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
    return Constants::INDEX_ONE;
  }

  int lua_getNetworkPayload(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    if (!manager || !name) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }

    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }

    auto* net = dynamic_cast<Project::Components::NetworkComponent*>(
        entity->getComponent(Components::NETWORK_COMPONENT));
    if (!net) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }

    const std::string& payload = net->getLastPayload();
    if (payload.empty()) {
      lua_pushnil(L);
    } else {
      lua_pushlstring(L, payload.c_str(), payload.size());
    }
    return Constants::INDEX_ONE;
  }

  int lua_getVisibleEntities(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    if (!manager || !name) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }

    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
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
      return Constants::INDEX_ONE;
    }

    lua_newtable(L);
    int index = Constants::INDEX_ONE;
    for (const auto* target : unique) {
      lua_pushstring(L, target->getEntityID().c_str());
      lua_rawseti(L, -Constants::INDEX_TWO, index++);
    }

    return Constants::INDEX_ONE;
  }

  int lua_setNetworkConnection(lua_State* L) {
    using namespace Project::Libraries::Categories::Protocols;

    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    const char* endpoint = luaL_checkstring(L, Constants::INDEX_TWO);
    const char* protocolStr = luaL_optstring(L, Constants::INDEX_THREE, HTTP);
    const char* tokenKey = luaL_optstring(L, Constants::INDEX_FOUR, Constants::EMPTY_STRING);
    if (!manager || !name || !endpoint) {
      return 0;
    }

    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }
    if (!entity) return 0;

    auto* net = dynamic_cast<Project::Components::NetworkComponent*>(
        entity->getComponent(Components::NETWORK_COMPONENT));
    if (!net) return 0;

    net->setEndpoint(endpoint);
    net->setProtocol(Project::Services::NetworkProtocolResolver::resolve(protocolStr));
    if (tokenKey && *tokenKey) net->setTokenKey(tokenKey);
    return 0;
  }

  int lua_setEntityText(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    const char* text = luaL_checkstring(L, Constants::INDEX_TWO);
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
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    int active = lua_toboolean(L, Constants::INDEX_TWO);
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
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    int active = lua_toboolean(L, Constants::INDEX_TWO);
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
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
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
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
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
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
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
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
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
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
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
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
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

    float value = static_cast<float>(luaL_checknumber(L, Constants::INDEX_THREE));
    float limit = 0.0f;
    if (lua_gettop(L) >= Constants::INDEX_FOUR && lua_isnumber(L, Constants::INDEX_FOUR)) {
      limit = static_cast<float>(lua_tonumber(L, Constants::INDEX_FOUR));
      numeric->setLimit(key, limit);
    }
    numeric->setValue(key, value);
    return 0;
  }

  int lua_getNumericValue(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
    const char* key = luaL_checkstring(L, Constants::INDEX_TWO);
    if (!manager || !name || !key) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }
    auto* numeric = dynamic_cast<Project::Components::NumericComponent*>(entity->getComponent(Components::NUMERIC_COMPONENT));
    if (!numeric || !numeric->has(key)) {
      lua_pushnil(L);
      return Constants::INDEX_ONE;
    }

    lua_pushnumber(L, numeric->getValue(key));
      return Constants::INDEX_ONE;
  }

  int lua_stopTimer(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(Constants::INDEX_ONE)));
    const char* name = luaL_checkstring(L, Constants::INDEX_ONE);
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
}
