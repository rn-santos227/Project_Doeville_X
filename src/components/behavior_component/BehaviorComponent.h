#ifndef BEHAVIOR_COMPONENT_H
#define BEHAVIOR_COMPONENT_H

#include <string>

#include "components/BaseComponent.h"
#include "handlers/behavior/BehaviorHandler.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class BehaviorComponent : public BaseComponent {
  public:
    explicit BehaviorComponent(Project::Utilities::LogsManager& logsManager);
    ~BehaviorComponent() override = default;

    Project::Entities::Entity* getOwner() const override { return owner; }
    ComponentType getType() const override { return ComponentType::BEHAVIOR; }

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }
    Project::Handlers::BehaviorHandler& getHandler() { return handler; }

  private:
    Project::Entities::Entity* owner = nullptr;
    Project::Handlers::BehaviorHandler handler;
    std::string scriptPath;
  };
}

#endif
