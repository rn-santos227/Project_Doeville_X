#ifndef BOUNDING_BOX_COMPONENT_H
#define BOUNDING_BOX_COMPONENT_H

#include "SurfaceType.h"

#include <SDL.h>
#include <vector>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "libraries/constants/Constants.h"
#include "handlers/input/KeyHandler.h"
#include "handlers/camera/CameraHandler.h"
#include "interfaces/rotation_interface/Rotatable.h"
#include "utilities/geometry/GeometryUtils.h"

namespace Project::Entities { class Entity; }

namespace Project::Components {
  class BoundingBoxComponent : public BaseComponent, public PositionableComponent, public Project::Interfaces::Rotatable {
  public:
    explicit BoundingBoxComponent(Project::Utilities::LogsManager& logsManager, SDL_Renderer* renderer, Project::Handlers::KeyHandler* keyHandler, SDL_Color debugColor);
    static void setCameraHandler(Project::Handlers::CameraHandler* handler);

    Project::Entities::Entity* getOwner() const override { return owner; }

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void addBox(const SDL_Rect& rect);
    void addCircle(int x, int y, int r);
    void addPolygon(const std::vector<SDL_FPoint>& points);
    void addCapsule(const Project::Utilities::Capsule& cap);
    
    const std::vector<SDL_Rect>& getBoxes() const;
    const std::vector<Project::Utilities::Circle>& getCircles() const;
    const std::vector<Project::Utilities::OrientedBox>& getOrientedBoxes() const;
    const std::vector<Project::Utilities::Polygon>& getPolygons() const { return worldPolygons; }
    const std::vector<Project::Utilities::Capsule>& getCapsules() const { return worldCapsules; }

    void setSolid(bool solidEnabled);
    bool isSolid() const;
    bool isInteractive() const;

    bool handleSurfaceInteraction(
      Project::Components::SurfaceType surface,
      Project::Entities::Entity* target,
      const SDL_FPoint& offset,
      float bounce, float friction,
      float& velocityX, float& velocityY
    );

    void setFriction(float value) { friction = value; }
    float getFriction() const { return friction; }

    void setRotation(float angle) { rotation = angle; updateWorldBoxes(); }
    float getRotation() const { return rotation; }

    void setRotationEnabled(bool value) { rotationEnabled = value; updateWorldBoxes(); }
    bool isRotationEnabled() const { return rotationEnabled; }
    
    void setRestitution(float value) { restitution = value; }
    float getRestitution() const { return restitution; }

    void setEntityPosition(int x, int y) override;
    void setEntityRotation(float angle) override;

    void setSurfaceType(Project::Components::SurfaceType type) { surfaceType = type; }
    Project::Components::SurfaceType getSurfaceType() const { return surfaceType; }

    void clearShapes();
    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }

  private:
    static Project::Handlers::CameraHandler* cameraHandler;
    
    Project::Components::SurfaceType surfaceType = Project::Components::SurfaceType::REST;
    Project::Entities::Entity* owner = nullptr;

    std::vector<SDL_Rect> boxes;
    std::vector<SDL_Rect> worldBoxes;
    std::vector<Project::Utilities::Circle> circles;
    std::vector<Project::Utilities::Circle> worldCircles;
    std::vector<Project::Utilities::OrientedBox> orientedBoxes;
    std::vector<Project::Utilities::Polygon> polygons;
    std::vector<Project::Utilities::Polygon> worldPolygons;
    std::vector<Project::Utilities::Capsule> capsules;
    std::vector<Project::Utilities::Capsule> worldCapsules;

    SDL_Renderer* renderer = nullptr;
    Project::Handlers::KeyHandler* keyHandler = nullptr;

    SDL_Color debugColor = Project::Libraries::Constants::DEFAULT_BOUNDING_BOX_COLOR;

    mutable float cachedSin = 0.0f;
    mutable float cachedCos = 1.0f;
    mutable float lastCachedRotation = std::numeric_limits<float>::quiet_NaN();

    float friction = Project::Libraries::Constants::DEFAULT_FRICTION;
    float restitution = Project::Libraries::Constants::DEFAULT_BOUNCE_FACTOR;
    float rotation = 0.0f;

    float entityX = 0;
    float entityY = 0;
    
    mutable bool worldBoxesDirty = true;
    bool rotationEnabled = false;
    bool solid = false;

    void ensureUpdated() const;
    void markDirty();
    void updateWorldBoxes();
  };
}

#endif
