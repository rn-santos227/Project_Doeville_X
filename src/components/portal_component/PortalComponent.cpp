#include "PortalComponent.h"

#include <SDL.h>

#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/camera_component/CameraComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "handlers/animation/AnimationHandler.h"
#include "libraries/constants/NameConstants.h"
#include "libraries/keys/LuaPropertyKeys.h"
#include "entities/EntitiesManager.h"
#include "states/GameState.h"

namespace Project::Components {
  namespace Keys = Project::Libraries::Keys;
  PortalComponent::PortalComponent(Project::Utilities::LogsManager& logsManager)
  : BaseComponent(logsManager) {}

  void PortalComponent::update(float deltaTime) {
    if (!isActive()) return;
    
    if (teleportPending) {
      elapsed += deltaTime;
      if (elapsed >= data.delay) {
        teleport(pendingEntity);
        teleportPending = false;
        pendingEntity = nullptr;
        elapsed = 0.0f;
      }
      return;
    }

    checkTriggers();
  }

  void PortalComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    data.targetX = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::TARGET_X, 0.0f));
    data.targetY = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::TARGET_Y, 0.0f));

    data.delay = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::DELAY, 0.0f));
    
    data.triggerOnAction = luaStateWrapper.getTableBoolean(tableName, Keys::TRIGGER_ON_ACTION, false);
    data.triggerOnInside = luaStateWrapper.getTableBoolean(tableName, Keys::TRIGGER_ON_INSIDE, false);
    data.triggerOnTouch = luaStateWrapper.getTableBoolean(tableName, Keys::TRIGGER_ON_TOUCH, false);
    
    data.animation = luaStateWrapper.getTableString(tableName, Keys::ANIMATION, Project::Libraries::Constants::EMPTY_STRING);
  }

  void PortalComponent::trigger(Project::Entities::Entity* entity) {
    if (!isActive() || !entity || !data.triggerOnAction) return;
    beginTeleport(entity);
  }

  void PortalComponent::checkTriggers() {
    if (!owner) return;
    if (!data.triggerOnInside && !data.triggerOnTouch) return;

    auto* portalBox = owner->getBoundingBoxComponent();
    if (!portalBox) return;
    const auto& portalBoxes = portalBox->getBoxes();
    if (portalBoxes.empty()) return;
    SDL_Rect portalRect = portalBoxes.front();

    auto* manager = owner->getEntitiesManager();
    if (!manager) return;
    auto entities = manager->filterEntitiesByComponents({ComponentType::BOUNDING_BOX});
    for (auto* ent : entities) {
      if (!ent || ent == owner) continue;
      auto* bbox = ent->getBoundingBoxComponent();
      if (!bbox) continue;
      const auto& boxes = bbox->getBoxes();
      if (boxes.empty()) continue;
      SDL_Rect rect = boxes.front();
      bool inside = rect.x >= portalRect.x && rect.y >= portalRect.y &&
        rect.x + rect.w <= portalRect.x + portalRect.w &&
        rect.y + rect.h <= portalRect.y + portalRect.h;
      bool overlap = SDL_HasIntersection(&rect, &portalRect);
      if ((data.triggerOnInside && inside) || (data.triggerOnTouch && overlap)) {
        beginTeleport(ent);
        break;
      }
    }
  }
}
