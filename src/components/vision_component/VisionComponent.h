#ifndef VISION_COMPONENT_H
#define VISION_COMPONENT_H

#include "VisionData.h"

#include <vector>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "interfaces/rotation_interface/Rotatable.h"
#include "entities/EntitiesManager.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Components {
  class VisionComponent : public BaseComponent, public PositionableComponent, public Project::Interfaces::Rotatable {
  public:
    VisionComponent(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager);
    ComponentType getType() const override { return ComponentType::VISION; }

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void setEntityPosition(float x, float y) override { data.position = {x, y}; }
    void setEntityRotation(float _angle) override { data.direction = _angle; }

    void setShape(VisionShape _shape) { data.shape = _shape; }
    void setRadius(float _radius) { data.radius = _radius; }
    void setAngle(float _angle) { data.angle = _angle; }
    void setDirection(float _direction) { data.direction = _direction; }
    void setRayCount(int _rays) { data.rays = _rays; }
    
    void setRevealDarkness(bool _vision) { data.revealDarkness = _vision; }
    bool doesRevealDarkness() const { return data.revealDarkness; }

    const std::vector<SDL_FPoint>& getRayEndpoints() const { return endpoints; }
    const std::vector<Project::Entities::Entity*>& getVisibleEntities() const { return visibleEntities; }
    bool canSee(const Project::Entities::Entity* target) const;

    void setEntityReference(Project::Entities::Entity* entity);
    Project::Entities::Entity* getOwner() const override { return owner; }

    void renderMask(SDL_Renderer* target);

  private:
    SDL_Renderer* renderer;
    Project::Entities::EntitiesManager* entitiesManager = nullptr;
    Project::Entities::Entity* owner = nullptr;
    VisionData data;

    std::vector<SDL_FPoint> endpoints;
    std::vector<Project::Entities::Entity*> visibleEntities;

    std::pair<SDL_FPoint, Project::Entities::Entity*> castSingleRay(float angleDegrees);
    bool rayIntersectsAABB(const float origin[2], const float dir[2], const SDL_FRect& rect, float& outT) const;
    void castRays();
  };
}

#endif
