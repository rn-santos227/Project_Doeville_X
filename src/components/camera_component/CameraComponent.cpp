#include "CameraComponent.h"

#include "entities/Entity.h"
#include "libraries/keys/Keys.h"

namespace Project::Components {
  namespace Keys = Project::Libraries::Keys;

  CameraComponent::CameraComponent(Project::Utilities::LogsManager& logsManager, Project::Handlers::CameraHandler* handler)
    : BaseComponent(logsManager), cameraHandler(handler) {}

  void CameraComponent::update(float /*deltaTime*/) {
    if (!owner || !cameraHandler) return;
    int camX = static_cast<int>(owner->getX() - cameraHandler->getWidth() / 2);
    int camY = static_cast<int>(owner->getY() - cameraHandler->getHeight() / 2);
    cameraHandler->setPosition(camX, camY);
  }

  void CameraComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    //saving this for something else.
  }
}
