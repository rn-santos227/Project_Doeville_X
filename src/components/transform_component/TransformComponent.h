#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include <vector>

#include "components/BaseComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "libraries/constants/Constants.h"
#include "libraries/categories/Categories.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class TransformComponent : public BaseComponent {
  public:
    TransformComponent(Project::Utilities::LogsManager& logsManager, float flexibility = Project::Libraries::Constants::DEFAULT_HALF, float spin = 0.0f, bool allowRevert = true);
    ~TransformComponent() override = default;

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }
    void setFlexibility(float value) { flexibility = value; }
    float getFlexibility() const { return flexibility; }

    void setSpin(float value) { spin = value; }
    float getSpin() const { return spin; }

    void setAllowRevert(bool value) { allowRevert = value; }
    bool canRevert() const { return allowRevert; }

    bool isTransformedState() const { return transformed; }

  private:
    Project::Entities::Entity* owner = nullptr;
    std::vector<SDL_Rect> originalBoxes;
    std::vector<Project::Utilities::Circle> originalCircles;

    float flexibility;
    float spin;
    bool allowRevert;
    bool transformed = false;

    void transform();
    void revert();
  };
}

#endif
