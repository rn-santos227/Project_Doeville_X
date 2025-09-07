#include "NetworkComponent.h"

#include <sstream>

#include "entities/Entity.h"
#include "libraries/keys/Keys.h"
#include "services/network/NetworkProtocolResolver.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"
#include "watchers/network/NetworkWatcher.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  using Project::Services::NetworkService;
  using Project::Services::NetworkProtocolResolver;

  namespace Keys = Project::Libraries::Keys;
  NetworkComponent::NetworkComponent(LogsManager& logsManager, NetworkService& service)
    : BaseComponent(logsManager), network(service) {
    network.onPayload([this](const NetworkService::Payload& p) {
      applyPayload(p);
    });
  }

  void NetworkComponent::update(float) {
    if (!isActive() || data.connected || data.endpoint.empty()) return;
    if (network.connect(data.endpoint, data.protocol, data.tokenKey))
      data.connected = true;
  }
}
