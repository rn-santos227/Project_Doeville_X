#ifndef NETWORK_COMPONENT_H
#define NETWORK_COMPONENT_H

#include "NetworkData.h"

#include "components/BaseComponent.h"
#include "services/network/NetworkService.h"

namespace Project::Watchers { class NetworkWatcher; }
namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class NetworkComponent : public BaseComponent {
  
  private:
    Project::Entities::Entity* owner{nullptr};
    Project::Services::NetworkService& network;
    NetworkData data;
    std::string lastPayload;
  };
}

#endif
