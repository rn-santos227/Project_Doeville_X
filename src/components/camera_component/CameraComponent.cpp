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
    Project::Entities::Entity* focus = target ? target : owner;
    if (!focus) return;

    auto* mgr = focus->getEntitiesManager();
    float minZoom = Constants::DEFAULT_CAMERA_MIN_ZOOM;
    bool clampX = false;
    bool clampY = false;
    SDL_Rect map{0, 0, 0, 0};
    Project::States::DimensionMode mode = Project::States::DimensionMode::BOXED;

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
          clampX = map.w > 0 && (mode == Project::States::DimensionMode::BOUNDED || map.w > static_cast<int>(cameraHandler->getWidth()));
          clampY = map.h > 0 && (mode == Project::States::DimensionMode::BOUNDED || map.h > static_cast<int>(cameraHandler->getHeight()));
          if (mode == Project::States::DimensionMode::MAPPED && map.w > 0 && map.h > 0) {
            float viewW = static_cast<float>(cameraHandler->getViewportWidth());
            float viewH = static_cast<float>(cameraHandler->getViewportHeight());
            float minZoomX = viewW / static_cast<float>(map.w);
            float minZoomY = viewH / static_cast<float>(map.h);
            float mappedMin = std::min(Constants::DEFAULT_WHOLE, std::min(minZoomX, minZoomY));
            minZoom = std::max(minZoom, mappedMin);
          }
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
    targetZoom = z;
    if (cameraHandler) {
      cameraHandler->setZoom(z);
      data.zoom = cameraHandler->getZoom();
    } else {
      data.zoom = z;
    }
  }

  void CameraComponent::zoomIn(float amount) {
    targetZoom += amount;
  }

  void CameraComponent::zoomOut(float amount) {
    targetZoom -= amount;
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
