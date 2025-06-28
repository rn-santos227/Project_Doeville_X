#include "BoundingBoxComponent.h"

namespace Project::Components {
  BoundingBoxComponent::BoundingBoxComponent(LogsManager& logsManager)
    : BaseComponent(logsManager) {}

  void BoundingBoxComponent::update(float /*deltaTime*/) {}
}
