#ifndef SPAWNER_COMPONENT_H
#define SPAWNER_COMPONENT_H

#include "components/BaseComponent.h"

#include <lua.hpp>
#include <string>

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class SpawnerComponent : public BaseComponent {
  public:
    explicit SpawnerComponent(Project::Utilities::LogsManager& logsManager);
    ~SpawnerComponent() override = default;

    void update(float) override {}
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void setEntityReference(Project::Entities::Entity* entity);
    void setTemplate(const std::string& _name) { templateName = _name; }

    void spawn(float _offsetX, float _offsetY, float _velocityX, float _velocityY);

  private:
    Project::Entities::Entity* owner = nullptr;
    std::string templateName;
  };
}

#endif
