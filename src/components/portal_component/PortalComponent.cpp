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
}
