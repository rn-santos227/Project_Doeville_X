#include "NetworkComponent.h"

#include <sstream>

#include "components/numeric_component/NumericComponent.h"
#include "entities/Entity.h"
#include "libraries/keys/Keys.h"
#include "libraries/categories/ComponentCategories.h"
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
    if (!isActive() || data.endpoint.empty()) return;

    if (!data.connected) {

    }
  }

  void NetworkComponent::build(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    using namespace Project::Libraries::Constants;
    using namespace Project::Libraries::Categories::Protocols;

    data.endpoint = luaStateWrapper.getTableString(tableName, Keys::ENDPOINT, EMPTY_STRING);
    auto protoStr = luaStateWrapper.getTableString(tableName, Keys::PROTOCOL, HTTP);
    data.protocol = NetworkProtocolResolver::resolve(protoStr);
    data.tokenKey = luaStateWrapper.getTableString(tableName, Keys::TOKEN_KEY, EMPTY_STRING);
  }

  void NetworkComponent::subscribe(Project::Watchers::NetworkWatcher& watcher) {
    watcher.subscribe();
  }

  void NetworkComponent::applyPayload(const Project::Services::NetworkService::Payload& payload) {
    using namespace Project::Libraries::Categories::Components;

    if (!owner) return;
    std::string msg(payload.begin(), payload.end());
    std::istringstream stream(msg);
    std::string id;
    if (!std::getline(stream, id, ':')) return;
    if (id != owner->getEntityID()) return;

    lastPayload = msg;

    std::string data;
    if (!std::getline(stream, data)) return;

    auto equalPos = data.find('=');
    if (equalPos != std::string::npos) {
      std::string key = data.substr(0, equalPos);
      float value = std::stof(data.substr(equalPos + 1));
      if (auto* base = owner->getComponent(NUMERIC_COMPONENT)) {
        if (auto* numeric = dynamic_cast<NumericComponent*>(base)) {
          numeric->setValue(key, value);
        }
      }
      return;
    }

    float x = owner->getX(), y = owner->getY(), z = owner->getZ();
    char comma;

    std::istringstream coords(data);
    if (coords >> x >> comma >> y) {
      if (coords >> comma >> z)
        owner->setPosition(x, y, z);
      else
        owner->setPosition(x, y);
    }
  }
}
