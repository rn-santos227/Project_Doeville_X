#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "LightData.h"

#include <vector>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "utilities/logs_manager/LogsManager.h"
#include "entities/EntitiesManager.h"

namespace Project::Components {
  class LightComponent : public BaseComponent, public PositionableComponent {
  public:
    LightComponent(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager);
    ComponentType getType() const override { return ComponentType::LIGHT; }

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void setEntityPosition(float x, float y) override { data.position = {x, y}; }

    void setShape(LightShape _shape) { data.shape = _shape; }
    void setRadius(float _radius) { data.radius = _radius; }
    void setAngle(float _angle) { data.angle = _angle; }
    void setDirection(float _direction) { data.direction = _direction; }
    void setRayCount(int _rays) { data.rays = _rays; }
    void setColor(SDL_Color _color) { data.color = _color; }

    const std::vector<SDL_FPoint>& getRayEndpoints() const { return endpoints; }

    void setEntityReference(Project::Entities::Entity* entity);
    Project::Entities::Entity* getOwner() const override { return owner; }

  private:
    SDL_Renderer* renderer;
    Project::Entities::EntitiesManager* entitiesManager = nullptr;
    Project::Entities::Entity* owner = nullptr;
    LightData data;
    
    std::vector<SDL_FPoint> endpoints;

    SDL_FPoint castSingleRay(float angleDegrees);
    void castRays();
  };
}

#endif
