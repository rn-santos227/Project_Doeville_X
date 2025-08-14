#ifndef PORTAL_COMPONENT_H
#define PORTAL_COMPONENT_H

#include "PortalData.h"
#include <string>
#include <SDL.h>
#include "components/BaseComponent.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class PortalComponent : public BaseComponent {
  public:
    explicit PortalComponent(Project::Utilities::LogsManager& logsManager);
    ~PortalComponent() override = default;

    Project::Entities::Entity* getOwner() const override { return owner; }
    ComponentType getType() const override { return ComponentType::PORTAL; }

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }
    void setTarget(float x, float y) { data.targetX = x; data.targetY = y; }
    void setDelay(float value) { data.delay = value; }
    void setAnimation(const std::string& name) { data.animation = name; }

    void trigger(Project::Entities::Entity* entity);

  private:
    Project::Entities::Entity* owner = nullptr;
    PortalData data;
    
    Project::Entities::Entity* pendingEntity = nullptr;
    float elapsed = 0.0f;
    bool teleportPending = false;

    void checkTriggers();
    void beginTeleport(Project::Entities::Entity* entity);
    void teleport(Project::Entities::Entity* entity);
  };
}

#endif
