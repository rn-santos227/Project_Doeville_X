#include "CameraComponent.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "libraries/constants/FloatConstants.h"
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
    targetZoom = data.zoom;

    if (isActive() && owner) {
      if (auto* mgr = owner->getEntitiesManager()) {
        if (auto* state = mgr->getGameState()) {
          state->setActiveCamera(this);
        }
      }
    }
  }

  void CameraComponent::update(float deltaTime) {
    if (!cameraHandler) return;

    if (data.zoom != targetZoom) {
      float diff = targetZoom - data.zoom;
      float step = data.zoomSpeed * deltaTime;
      if (std::abs(diff) <= step) {
        data.zoom = targetZoom;
      } else {
        data.zoom += (diff > 0 ? step : -step);
      }
      cameraHandler->setZoom(data.zoom);
    }

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
        if (mode == Project::States::DimensionMode::BOUNDED || mode == Project::States::DimensionMode::MAPPED) {
          map = state->getMapRect();
          clamp = map.w > 0 && map.h > 0;
        }
      }
    }

    float angle = data.rotation;
    float offsetX = data.offsetX * std::cos(angle) - data.offsetY * std::sin(angle);
    float offsetY = data.offsetX * std::sin(angle) + data.offsetY * std::cos(angle);

    float desiredXF = focus->getX() + offsetX + Constants::DEFAULT_COMPONENT_SIZE - static_cast<float>(cameraHandler->getWidth()) / Constants::INDEX_TWO;
    float desiredYF = focus->getY() + offsetY + Constants::DEFAULT_COMPONENT_SIZE - static_cast<float>(cameraHandler->getHeight()) / Constants::INDEX_TWO;

    if (clamp) {
      float maxX = static_cast<float>(map.x + map.w - cameraHandler->getWidth());
      float maxY = static_cast<float>(map.y + map.h - cameraHandler->getHeight());
      desiredXF = std::clamp(desiredXF, static_cast<float>(map.x), maxX);
      desiredYF = std::clamp(desiredYF, static_cast<float>(map.y), maxY);
    }

    if (!havePos) {
      camXF = cameraHandler->getX();
      camYF = cameraHandler->getY();
      havePos = true;
    }

    float t = std::min(Constants::DEFAULT_WHOLE, data.followSpeed * deltaTime);
    camXF = Project::Utilities::MathUtils::lerp(camXF, desiredXF, t);
    camYF = Project::Utilities::MathUtils::lerp(camYF, desiredYF, t);

    if (shakeTime > 0.0f) {
      float shakeX = ((static_cast<float>(std::rand()) / RAND_MAX) * Constants::DEFAULT_DOUBLE - Constants::DEFAULT_WHOLE) * data.shakeIntensity;
      float shakeY = ((static_cast<float>(std::rand()) / RAND_MAX) * Constants::DEFAULT_DOUBLE - Constants::DEFAULT_WHOLE) * data.shakeIntensity;
      camXF += shakeX;
      camYF += shakeY;
      shakeTime -= deltaTime;
    }

    if (clamp) {
      float maxX = static_cast<float>(map.x + map.w - cameraHandler->getWidth());
      float maxY = static_cast<float>(map.y + map.h - cameraHandler->getHeight());
      camXF = std::clamp(camXF, static_cast<float>(map.x), maxX);
      camYF = std::clamp(camYF, static_cast<float>(map.y), maxY);
    }

    cameraHandler->setPosition(camXF, camYF);
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
    data.shakeIntensity = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::INTENSITY, Constants::DEFAULT_CAMERA_SHAKE_INTENSITY));

    shakeTime = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::DURATION, 0.0));

    data.targetName = luaStateWrapper.getTableString(tableName, Keys::TARGET, Constants::EMPTY_STRING);
    if (cameraHandler) {
      cameraHandler->setZoom(data.zoom);
      cameraHandler->setRotation(data.rotation);
    }
  }

  void CameraComponent::snapToTarget() {
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
        if (mode == Project::States::DimensionMode::BOUNDED ||
            mode == Project::States::DimensionMode::MAPPED) {
          map = state->getMapRect();
          clamp = map.w > 0 && map.h > 0;
        }
      }
    }

    float angle = data.rotation;
    float offsetX = data.offsetX * std::cos(angle) - data.offsetY * std::sin(angle);
    float offsetY = data.offsetX * std::sin(angle) + data.offsetY * std::cos(angle);

    camXF = focus->getX() + offsetX + Constants::DEFAULT_COMPONENT_SIZE - static_cast<float>(cameraHandler->getWidth()) / Constants::INDEX_TWO;
    camYF = focus->getY() + offsetY + Constants::DEFAULT_COMPONENT_SIZE - static_cast<float>(cameraHandler->getHeight()) / Constants::INDEX_TWO;

    if (clamp) {
      float maxX = static_cast<float>(map.x + map.w - cameraHandler->getWidth());
      float maxY = static_cast<float>(map.y + map.h - cameraHandler->getHeight());
      camXF = std::clamp(camXF, static_cast<float>(map.x), maxX);
      camYF = std::clamp(camYF, static_cast<float>(map.y), maxY);
    }

    cameraHandler->setPosition(camXF, camYF);
    havePos = true;
  }

  void CameraComponent::rotate(float amount) {
    setRotation(data.rotation + amount);
  }

  void CameraComponent::shake(float duration) {
    shakeTime = duration;
  }

  void CameraComponent::setZoom(float z) {
    targetZoom = std::clamp(z, Constants::DEFAULT_CAMERA_MIN_ZOOM, Constants::DEFAULT_CAMERA_MAX_ZOOM);
    if (cameraHandler) {
      cameraHandler->setZoom(targetZoom);
      data.zoom = cameraHandler->getZoom();
    } else {
      data.zoom = targetZoom;
    }
  }

  void CameraComponent::zoomIn(float amount) {
    targetZoom = std::clamp(targetZoom + amount, Constants::DEFAULT_CAMERA_MIN_ZOOM, Constants::DEFAULT_CAMERA_MAX_ZOOM);
  }

  void CameraComponent::zoomOut(float amount) {
    targetZoom = std::clamp(targetZoom - amount, Constants::DEFAULT_CAMERA_MIN_ZOOM, Constants::DEFAULT_CAMERA_MAX_ZOOM);
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

  void CameraComponent::clampToMap(float& x, float& y, const SDL_Rect& map) {
    float viewW = static_cast<float>(cameraHandler->getWidth());
    float viewH = static_cast<float>(cameraHandler->getHeight());
    float minX = static_cast<float>(map.x);
    float minY = static_cast<float>(map.y);
    float maxX = static_cast<float>(map.x + map.w - cameraHandler->getWidth());
    float maxY = static_cast<float>(map.y + map.h - cameraHandler->getHeight());
    if (viewW > static_cast<float>(map.w)) {
      x = minX + (static_cast<float>(map.w) - viewW) / Constants::INDEX_TWO;
    } else {
      x = std::clamp(x, minX, maxX);
    }

    if (viewH > static_cast<float>(map.h)) {
      y = minY + (static_cast<float>(map.h) - viewH) / Constants::INDEX_TWO;
    } else {
      y = std::clamp(y, minY, maxY);
    }
  }
}
