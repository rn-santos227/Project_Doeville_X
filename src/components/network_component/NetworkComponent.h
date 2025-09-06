#ifndef NETWORK_COMPONENT_H
#define NETWORK_COMPONENT_H

#include "NetworkData.h"

#include "components/BaseComponent.h"
#include "services/network/NetworkService.h"

namespace Project::Watchers { class NetworkWatcher; }
namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class NetworkComponent : public BaseComponent {
    NetworkComponent(Project::Utilities::LogsManager& logsManager, Project::Services::NetworkService& service);
    ~NetworkComponent() override = default;
    
  private:
    Project::Entities::Entity* owner{nullptr};
    Project::Services::NetworkService& network;
    NetworkData data;
    std::string lastPayload;

    void applyPayload(const Project::Services::NetworkService::Payload& payload);
  };
}

#endif
