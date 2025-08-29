#ifndef BOUNDING_BOX_COMPONENT_H
#define BOUNDING_BOX_COMPONENT_H

#include "BoundingBoxData.h"
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
    ComponentType getType() const override { return ComponentType::BOUNDING_BOX; }

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void addBox(const SDL_FRect& rect);
    void addCircle(float x, float y, float r);
    void addPolygon(const std::vector<SDL_FPoint>& points);
    void addCapsule(const Project::Utilities::Capsule& cap);
    
    const std::vector<SDL_FRect>& getBoxes() const;
    const std::vector<Project::Utilities::Circle>& getCircles() const;
    const std::vector<Project::Utilities::OrientedBox>& getOrientedBoxes() const;
    const std::vector<Project::Utilities::Polygon>& getPolygons() const { return worldPolygons; }
    const std::vector<Project::Utilities::Capsule>& getCapsules() const { return worldCapsules; }

    bool isInteractive() const;

    bool handleSurfaceInteraction(
      Project::Components::SurfaceType surface,
      Project::Entities::Entity* target,
      const SDL_FPoint& offset,
      float bounce, float friction,
      float& velocityX, float& velocityY
    );

    void setFriction(float value) { data.friction = value; }
    float getFriction() const { return data.friction; }

    void setRotation(float angle) { data.rotation = angle; updateWorldBoxes(); }
    float getRotation() const { return data.rotation; }

    void setRotationEnabled(bool value) { data.rotationEnabled = value; updateWorldBoxes(); }
    bool isRotationEnabled() const { return data.rotationEnabled; }

    void setSolid(bool solidEnabled) { data.solid = solidEnabled; }
    bool isSolid() const { return data.solid; }

    void setRestitution(float value) { data.restitution = value; }
    float getRestitution() const { return data.restitution; }

    void setEntityPosition(float x, float y) override;
    void setEntityRotation(float angle) override;

    void setSurfaceType(Project::Components::SurfaceType type) { data.surfaceType = type; }
    Project::Components::SurfaceType getSurfaceType() const { return data.surfaceType; }

    void setIgnoredEntities(const std::vector<std::string>& names);
    bool isIgnoring(const std::string& name) const;

    void clearShapes();
    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }

    void setUseProxy(bool v) { useProxy = v; }
    bool usesProxy() const { return useProxy; }
    SDL_FRect getProxyAABB() const;

  private:
    static Project::Handlers::CameraHandler* cameraHandler;
    BoundingBoxData data;
    
    Project::Entities::Entity* owner = nullptr;

    std::vector<SDL_FRect> worldBoxes;
    std::vector<Project::Utilities::Circle> worldCircles;
    std::vector<Project::Utilities::Polygon> worldPolygons;
    std::vector<Project::Utilities::Capsule> worldCapsules;
    std::unordered_set<std::string> ignoredEntities;

    SDL_Renderer* renderer = nullptr;
    Project::Handlers::KeyHandler* keyHandler = nullptr;
    
    mutable float cachedSin = 0.0f;
    mutable float cachedCos = 1.0f;
    mutable float lastCachedRotation = std::numeric_limits<float>::quiet_NaN();
    
    float entityX = 0;
    float entityY = 0;

    mutable bool worldBoxesDirty = true;
    bool useProxy = false;

    void ensureUpdated() const;
    void markDirty();
    void updateWorldBoxes();
  };
}

#endif
