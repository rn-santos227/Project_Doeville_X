#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "CameraData.h"

#include <string>

#include "components/BaseComponent.h"
#include "handlers/camera/CameraHandler.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class CameraComponent : public BaseComponent {
  public:
    CameraComponent(Project::Utilities::LogsManager& logsManager, Project::Handlers::CameraHandler* handler);
    ~CameraComponent() override = default;

    Project::Entities::Entity* getOwner() const override { return owner; }
    ComponentType getType() const override { return ComponentType::CAMERA; }

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void onAttach() override;

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; target = entity; }
    void setCameraHandler(Project::Handlers::CameraHandler* handler) { cameraHandler = handler; }

    void setZoom(float z);
    float getZoom() const { return data.zoom; }

    void setFollowSpeed(float speed) { data.followSpeed = speed; }
    float getFollowSpeed() const { return data.followSpeed; }

    void setRotation(float angle);
    float getRotation() const { return data.rotation; }

    void setSpinSpeed(float speed) { data.spinSpeed = speed; }
    float getSpinSpeed() const { return data.spinSpeed; }

    void setOffset(float x, float y);
    float getOffsetX() const { return data.offsetX; }
    float getOffsetY() const { return data.offsetY; }
    
    void switchTarget(Project::Entities::Entity* newTarget);
    void switchTarget(const std::string& entityId);

  private:
    Project::Entities::Entity* owner = nullptr;
    Project::Entities::Entity* target = nullptr;
    Project::Handlers::CameraHandler* cameraHandler = nullptr;
    CameraData data;
  };
}

#endif
