#include "PortalComponent.h"

#include <SDL.h>

#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "entities/EntitiesManager.h"
#include "states/GameState.h"
#include "components/camera_component/CameraComponent.h"
#include "handlers/animation/AnimationHandler.h"

namespace Project::Components {
  PortalComponent::PortalComponent(Project::Utilities::LogsManager& logsManager)
  : BaseComponent(logsManager) {}

  void PortalComponent::update(float deltaTime) {
    if (!isActive()) return;
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
