#include "BehaviorComponent.h"

#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::Components {
  BehaviorComponent::BehaviorComponent(Project::Utilities::LogsManager& logsManager)
  : BaseComponent(logsManager), handler(logsManager) {}

  void BehaviorComponent::update(float /*deltaTime*/) {
    handler.update();
  }
}
