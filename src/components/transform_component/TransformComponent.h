#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "TransformData.h"

#include <vector>

#include "components/BaseComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "interfaces/reset_interface/Resetable.h"
#include "libraries/constants/Constants.h"
#include "libraries/categories/Categories.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class TransformComponent : public BaseComponent, public Project::Interfaces::Resetable {
  public:
    TransformComponent(Project::Utilities::LogsManager& logsManager, float flexibility = Project::Libraries::Constants::DEFAULT_HALF, float spin = 0.0f, bool allowRevert = true);
    ~TransformComponent() override = default;

    Project::Entities::Entity* getOwner() const override { return owner; }
    ComponentType getType() const override { return ComponentType::TRANSFORM; }

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void reset() override;

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }
    void setFlexibility(float value) { data.flexibility = value; }
    float getFlexibility() const { return data.flexibility; }

    void setSpin(float value) { data.spin = value; }
    float getSpin() const { return data.spin; }

    void setAllowRevert(bool value) { data.allowRevert = value; }
    bool canRevert() const { return data.allowRevert; }

    bool isTransformedState() const { return data.transformed; }

  private:
    Project::Entities::Entity* owner = nullptr;
    TransformData data;

    void transform();
    void revert();
  };
}

#endif
