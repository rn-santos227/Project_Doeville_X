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
  
}
