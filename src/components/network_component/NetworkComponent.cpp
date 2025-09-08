#include "NetworkComponent.h"

#include <sstream>

#include "entities/Entity.h"
#include "libraries/keys/Keys.h"
#include "libraries/categories/ProtocolCategories.h"
#include "services/network/NetworkProtocolResolver.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"
#include "watchers/network/NetworkWatcher.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  using Project::Services::NetworkService;
  using Project::Services::NetworkProtocolResolver;
  using Project::Utilities::LuaStateWrapper;

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

  void NetworkComponent::build(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    using namespace Project::Libraries::Constants;
    using namespace Project::Libraries::Categories::Protocols;

    data.endpoint = luaStateWrapper.getTableString(tableName, Keys::ENDPOINT, EMPTY_STRING);
    auto protoStr = luaStateWrapper.getTableString(tableName, Keys::PROTOCOL, HTTP);
    data.protocol = NetworkProtocolResolver::resolve(protoStr);
    data.tokenKey = luaStateWrapper.getTableString(tableName, Keys::TOKEN_KEY, EMPTY_STRING);
  }

  void NetworkComponent::applyPayload(const Project::Services::NetworkService::Payload& payload) {
    if (!owner) return;
    std::string msg(payload.begin(), payload.end());
    std::istringstream stream(msg);
    std::string id;
    if (!std::getline(stream, id, ':')) return;
    if (id != owner->getEntityID()) return;

    lastPayload = msg;

    float x = owner->getX(), y = owner->getY(), z = owner->getZ();
    char comma;
  }
}
