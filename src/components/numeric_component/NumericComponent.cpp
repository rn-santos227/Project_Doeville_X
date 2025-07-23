#include "NumericComponent.h"

#include "libraries/keys/Keys.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;

  namespace Keys = Project::Libraries::Keys;

  NumericComponent::NumericComponent(LogsManager& logsManager)
    : BaseComponent(logsManager) {}

  void NumericComponent::update(float /*deltaTime*/) {}
}
