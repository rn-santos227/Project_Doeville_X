#ifndef NUMERIC_COMPONENT_H
#define NUMERIC_COMPONENT_H

#include <string>
#include <unordered_map>

#include "components/BaseComponent.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class NumericComponent : public BaseComponent {
  public:
    explicit NumericComponent(Project::Utilities::LogsManager& logsManager);
    ~NumericComponent() override = default;

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

  private:
    struct NumericValue {
      float value = 0.0f;
      float limit = 0.0f;
    };

    std::unordered_map<std::string, NumericValue> values;
    Project::Entities::Entity* owner = nullptr;
  };
}

#endif
