#include "CameraComponent.h"

#include "entities/Entity.h"
#include "entities/EntitiesManager.h"

#include "libraries/keys/Keys.h"
#include "states/GameState.h"

namespace Project::Components {
  namespace Keys = Project::Libraries::Keys;

  CameraComponent::CameraComponent(Project::Utilities::LogsManager& logsManager, Project::Handlers::CameraHandler* handler)
    : BaseComponent(logsManager), cameraHandler(handler) {}

  void CameraComponent::update(float /*deltaTime*/) {
    if (!owner || !cameraHandler) return;

    auto* mgr = owner->getEntitiesManager();
    if (mgr) {
      auto* state = mgr->getGameState();
      if (state && state->getDimensionMode() == Project::States::DimensionMode::BOXED) {
        return;
      }

      if (state && state->getDimensionMode() == Project::States::DimensionMode::WRAPPING) {
        return;
      }
    }

    int camX = static_cast<int>(owner->getX() - cameraHandler->getWidth() / 2);
    int camY = static_cast<int>(owner->getY() - cameraHandler->getHeight() / 2);
    cameraHandler->setPosition(camX, camY);
  }

  void CameraComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    //saving this for something else.
  }
}
