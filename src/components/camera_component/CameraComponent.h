#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "components/BaseComponent.h"
#include "handlers/camera/CameraHandler.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class CameraComponent : public BaseComponent {
  public:
    CameraComponent(Project::Utilities::LogsManager& logsManager, Project::Handlers::CameraHandler* handler);
    ~CameraComponent() override = default;

    Project::Entities::Entity* getOwner() const override { return owner; }

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }
    void setCameraHandler(Project::Handlers::CameraHandler* handler) { cameraHandler = handler; }

  private:
    Project::Entities::Entity* owner = nullptr;
    Project::Handlers::CameraHandler* cameraHandler = nullptr;
  };
}

#endif
