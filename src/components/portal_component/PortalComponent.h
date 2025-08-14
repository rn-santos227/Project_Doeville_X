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

  private:
    Project::Entities::Entity* owner = nullptr;
    PortalData data;
    
    Project::Entities::Entity* pendingEntity = nullptr;
    float elapsed = 0.0f;
    bool teleportPending = false;

    void checkTriggers();
    void teleport(Project::Entities::Entity* entity);
  };
}

#endif
