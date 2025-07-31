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

    Project::Entities::Entity* getOwner() const override { return owner; }
    ComponentType getType() const override { return ComponentType::NUMERIC; }

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }

    void setValue(const std::string& name, float value);
    float getValue(const std::string& name) const;

    void setLimit(const std::string& name, float limit);
    float getLimit(const std::string& name) const;

    void add(const std::string& name, float amount);
    bool has(const std::string& name) const;

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
