#ifndef BOUNDING_BOX_COMPONENT_H
#define BOUNDING_BOX_COMPONENT_H

#include <SDL.h>
#include <vector>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "libraries/constants/Constants.h"
#include "handlers/input/KeyHandler.h"
#include "interfaces/rotation_interface/Rotatable.h"
#include "utilities/geometry/GeometryUtils.h"

namespace Project::Components {
  struct OrientedBox {
    SDL_FPoint corners[Project::Libraries::Constants::INDEX_FOUR];
  };

  class BoundingBoxComponent : public BaseComponent, public PositionableComponent, public Project::Interfaces::Rotatable {
  public:
    using Circle = Project::Utilities::Circle;
    explicit BoundingBoxComponent(Project::Utilities::LogsManager& logsManager, SDL_Renderer* renderer, Project::Handlers::KeyHandler* keyHandler, SDL_Color debugColor);

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void addBox(const SDL_Rect& rect);
    void addCircle(int x, int y, int r);
    
    const std::vector<SDL_Rect>& getBoxes() const;
    const std::vector<Circle>& getCircles() const;
    const std::vector<OrientedBox>& getOrientedBoxes() const { return orientedBoxes; }

    void setSolid(bool solidEnabled);
    bool isSolid() const;

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

    void clearShapes();

  private:
    std::vector<SDL_Rect> boxes;
    std::vector<SDL_Rect> worldBoxes;
    std::vector<Circle> circles;
    std::vector<Circle> worldCircles;
    std::vector<OrientedBox> orientedBoxes;

    SDL_Renderer* renderer = nullptr;
    Project::Handlers::KeyHandler* keyHandler = nullptr;

    SDL_Color debugColor = Project::Libraries::Constants::DEFAULT_BOUNDING_BOX_COLOR;

    float friction = Project::Libraries::Constants::DEFAULT_FRICTION;
    float restitution = Project::Libraries::Constants::DEFAULT_BOUNCE_FACTOR;
    float rotation = 0.0f;

    float entityX = 0;
    float entityY = 0;
    
    bool rotationEnabled = false;
    bool solid = false;

    void updateWorldBoxes();
  };
}

#endif
