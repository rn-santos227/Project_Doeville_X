#ifndef BOUNDING_BOX_COMPONENT_H
#define BOUNDING_BOX_COMPONENT_H

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "libraries/constants/Constants.h"
#include "handlers/input/KeyHandler.h"
#include "utilities/geometry/GeometryUtils.h"

#include <SDL.h>
#include <vector>

namespace Project::Components {
  class BoundingBoxComponent : public BaseComponent, public PositionableComponent {
  public:
    using Circle = Project::Utilities::Circle;
    explicit BoundingBoxComponent(Project::Utilities::LogsManager& logsManager, SDL_Renderer* renderer, Project::Handlers::KeyHandler* keyHandler, SDL_Color debugColor);

    void update(float deltaTime) override;
    void render() override;

    void addBox(const SDL_Rect& rect);
    void addCircle(int x, int y, int r);
    
    const std::vector<SDL_Rect>& getBoxes() const;
    const std::vector<Circle>& getCircles() const;

    void setSolid(bool solidEnabled);
    bool isSolid() const;

    void setFriction(float value) { friction = value; }
    float getFriction() const { return friction; }
    
    void setRestitution(float value) { restitution = value; }
    float getRestitution() const { return restitution; }

    void setEntityPosition(int x, int y) override;
    void clearShapes();

  private:
    std::vector<SDL_Rect> boxes;
    std::vector<SDL_Rect> worldBoxes;
    std::vector<Circle> circles;
    std::vector<Circle> worldCircles;

    SDL_Renderer* renderer = nullptr;
    Project::Handlers::KeyHandler* keyHandler = nullptr;

    SDL_Color debugColor = Project::Libraries::Constants::DEFAULT_BOUNDING_BOX_COLOR;

    float friction = Project::Libraries::Constants::DEFAULT_FRICTION;
    float restitution = Project::Libraries::Constants::DEFAULT_BOUNCE_FACTOR;

    int entityX = 0;
    int entityY = 0;
    
    bool solid = false;

    void updateWorldBoxes();
  };
}

#endif
