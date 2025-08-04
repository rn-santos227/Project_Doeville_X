#include "CameraComponent.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "libraries/constants/IndexConstants.h"
#include "libraries/constants/NameConstants.h"
#include "libraries/keys/Keys.h"
#include "states/GameState.h"
#include "utilities/math/MathUtils.h"

namespace Project::Components {
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  CameraComponent::CameraComponent(Project::Utilities::LogsManager& logsManager, Project::Handlers::CameraHandler* handler)
    : BaseComponent(logsManager), cameraHandler(handler) {}

  void CameraComponent::onAttach() {
    target = owner;
    if (owner) {
      if (data.targetName.empty()) {
        data.targetName = owner->getEntityID();
      } else if (auto* mgr = owner->getEntitiesManager()) {
        auto ent = mgr->getEntity(data.targetName);
        if (ent) {
          target = ent.get();
        } else {
          data.targetName = owner->getEntityID();
        }
      }
    }
    if (cameraHandler) {
      cameraHandler->setZoom(data.zoom);
      cameraHandler->setRotation(data.rotation);
    }
  }

  void CameraComponent::update(float deltaTime) {
    if (!cameraHandler) return;
    Project::Entities::Entity* focus = target ? target : owner;
    if (!focus) return;

    auto* mgr = focus->getEntitiesManager();
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

    float angle = data.rotation;
    float offsetX = data.offsetX * std::cos(angle) - data.offsetY * std::sin(angle);
    float offsetY = data.offsetX * std::sin(angle) + data.offsetY * std::cos(angle);

    int desiredX = static_cast<int>(focus->getX() + offsetX + Constants::DEFAULT_COMPONENT_SIZE -
      cameraHandler->getWidth() / Constants::INDEX_TWO);
    int desiredY = static_cast<int>(focus->getY() + offsetY + Constants::DEFAULT_COMPONENT_SIZE -
      cameraHandler->getHeight() / Constants::INDEX_TWO);

    if (clamp) {
      desiredX = std::clamp(desiredX, map.x, map.x + map.w - cameraHandler->getWidth());
      desiredY = std::clamp(desiredY, map.y, map.y + map.h - cameraHandler->getHeight());
    }

    int currentX = cameraHandler->getX();
    int currentY = cameraHandler->getY();
    float t = std::min(Constants::DEFAULT_WHOLE, data.followSpeed * deltaTime);
    int camX = Project::Utilities::MathUtils::interpolate(currentX, desiredX, t);
    int camY = Project::Utilities::MathUtils::interpolate(currentY, desiredY, t);
    cameraHandler->setPosition(camX, camY);
    data.rotation += data.spinSpeed * deltaTime;
    cameraHandler->setRotation(data.rotation);
  }

  void CameraComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    data.zoom = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::ZOOM, Constants::DEFAULT_CAMERA_ZOOM));
    data.followSpeed = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::FOLLOW_SPEED, Constants::DEFAULT_CAMERA_SPEED));
    data.rotation = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::ROTATION, Constants::DEFAULT_CAMERA_ROTATION));
    data.spinSpeed = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::ROTATION_SPEED, Constants::DEFAULT_CAMERA_SPIN_SPEED));
    data.offsetX = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::OFFSET_X, 0.0));
    data.offsetY = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::OFFSET_Y, 0.0));
    
    data.targetName = luaStateWrapper.getTableString(tableName, Keys::TARGET, Constants::EMPTY_STRING);
    if (cameraHandler) {
      cameraHandler->setZoom(data.zoom);
      cameraHandler->setRotation(data.rotation);
    }
  }

  void CameraComponent::shake(float duration) {
    shakeTime = duration;
  }

  void CameraComponent::zoomIn(float amount) {
    setZoom(data.zoom + amount);
  }

  void CameraComponent::zoomOut(float amount) {
    setZoom(data.zoom - amount);
  }

  void CameraComponent::setRotation(float angle) {
    data.rotation = angle;
    if (cameraHandler) cameraHandler->setRotation(angle);
  }

  void CameraComponent::setOffset(float x, float y) {
    data.offsetX = x;
    data.offsetY = y;
  }
  
  void CameraComponent::switchTarget(Project::Entities::Entity* newTarget) {
    target = newTarget ? newTarget : owner;
  }

  void CameraComponent::switchTarget(const std::string& entityId) {
    if (!owner) return;
    if (auto* mgr = owner->getEntitiesManager()) {
      auto ent = mgr->getEntity(entityId);
      target = ent ? ent.get() : owner;
      data.targetName = entityId;
    }
  }
}
