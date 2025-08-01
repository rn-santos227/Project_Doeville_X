#include "CameraComponent.h"

#include <algorithm>

#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "libraries/constants/IndexConstants.h"
#include "libraries/keys/Keys.h"
#include "states/GameState.h"

namespace Project::Components {
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  CameraComponent::CameraComponent(Project::Utilities::LogsManager& logsManager, Project::Handlers::CameraHandler* handler)
    : BaseComponent(logsManager), cameraHandler(handler) {}

  void CameraComponent::update(float /*deltaTime*/) {
    if (!owner || !cameraHandler) return;

    auto* mgr = owner->getEntitiesManager();
    bool clamp = false;
    SDL_Rect map{0, 0, 0, 0};

    if (mgr) {
      auto* state = mgr->getGameState();
      if (state) {
        auto mode = state->getDimensionMode();
        if (mode == Project::States::DimensionMode::BOXED ||
            mode == Project::States::DimensionMode::WRAPPING) {
          return;
        }
        if (mode == Project::States::DimensionMode::BOUNDED) {
          map = state->getMapRect();
          clamp = map.w > 0 && map.h > 0;
        }
      }
    }

    int camX = static_cast<int>(owner->getX() - cameraHandler->getWidth() / Constants::INDEX_TWO);
    int camY = static_cast<int>(owner->getY() - cameraHandler->getHeight() / Constants::INDEX_TWO);

    if (clamp) {
      camX = std::clamp(camX, map.x, map.x + map.w - cameraHandler->getWidth());
      camY = std::clamp(camY, map.y, map.y + map.h - cameraHandler->getHeight());
    }

    cameraHandler->setPosition(camX, camY);
  }

  void CameraComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    //saving this for something else.
  }
}
