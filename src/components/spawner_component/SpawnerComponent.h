#ifndef SPAWNER_COMPONENT_H
#define SPAWNER_COMPONENT_H

#include "SpawnerData.h"

#include "components/BaseComponent.h"

#include <string>

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class SpawnerComponent : public BaseComponent {
  public:
    explicit SpawnerComponent(Project::Utilities::LogsManager& logsManager);
    ~SpawnerComponent() override = default;

    Project::Entities::Entity* getOwner() const override { return owner; }
    ComponentType getType() const override { return ComponentType::SPAWNER; }

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void setEntityReference(Project::Entities::Entity* entity);
    void setTemplate(const std::string& _name) { data.templateName = _name; }

    void spawn(float _offsetX, float _offsetY, float _velocityX, float _velocityY, float _rotation = 0.0f);

  private:
    Project::Entities::Entity* owner = nullptr;
    SpawnerData data;
  };
}

#endif
