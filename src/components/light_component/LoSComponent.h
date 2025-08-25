#ifndef LOS_COMPONENT_H
#define LOS_COMPONENT_H

#include "LoSData.h"

#include <vector>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "utilities/logs_manager/LogsManager.h"
#include "entities/EntitiesManager.h"

namespace Project::Components {
  class LoSComponent : public BaseComponent, public PositionableComponent {
  public:
    LoSComponent(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager);
    ComponentType getType() const override { return ComponentType::LOS; }

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void setEntityPosition(float x, float y) override { data.position = {x, y}; }

    void setShape(LoSShape s) { data.shape = s; }
    void setRadius(float r) { data.radius = r; }
    void setAngle(float a) { data.angle = a; }
    void setDirection(float d) { data.direction = d; }
    void setRayCount(int c) { data.rays = c; }
    void setRevealDarkness(bool v) { data.revealDarkness = v; }
    bool doesRevealDarkness() const { return data.revealDarkness; }

    const std::vector<SDL_FPoint>& getRayEndpoints() const { return endpoints; }
    const std::vector<Project::Entities::Entity*>& getVisibleEntities() const { return visibleEntities; }

    void setEntityReference(Project::Entities::Entity* entity);
    Project::Entities::Entity* getOwner() const override { return owner; }

    void renderMask(SDL_Renderer* target);

  private:
    SDL_Renderer* renderer;
    Project::Entities::EntitiesManager* entitiesManager = nullptr;
    Project::Entities::Entity* owner = nullptr;
    LoSData data;

    std::vector<SDL_FPoint> endpoints;
    std::vector<Project::Entities::Entity*> visibleEntities;

    void castRays();
    std::pair<SDL_FPoint, Project::Entities::Entity*> castSingleRay(float angleDegrees);
  };
}

#endif
