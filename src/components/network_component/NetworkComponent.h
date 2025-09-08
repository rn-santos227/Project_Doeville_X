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

    Project::Entities::Entity* getOwner() const override { return owner; }
    ComponentType getType() const override { return ComponentType::NETWORK; }

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }
    void setEndpoint(const std::string& endpoint) { data.endpoint = endpoint; }
    void setTokenKey(const std::string& tokenKey) { data.tokenKey = tokenKey; }
    void setProtocol(Project::Services::NetworkProtocol protocol) { data.protocol = protocol; }

    void subscribe(Project::Watchers::NetworkWatcher& watcher);
    const std::string& getLastPayload() const { return lastPayload; }

  private:
    Project::Entities::Entity* owner{nullptr};
    Project::Services::NetworkService& network;
    NetworkData data;
    std::string lastPayload;

    void applyPayload(const Project::Services::NetworkService::Payload& payload);
  };
}

#endif
