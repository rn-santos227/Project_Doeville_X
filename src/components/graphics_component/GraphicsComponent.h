#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include <string>
#include <SDL.h>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/TextureHolder.h"

#include "libraries/constants/Constants.h"

#include "handlers/animation/AnimationHandler.h"
#include "handlers/resources/ResourcesHandler.h"

#include "utilities/logs_manager/LogsManager.h"

namespace Constants = Project::Libraries::Constants;

namespace Project::Components  {
  class GraphicsComponent : public BaseComponent, public PositionableComponent, public TextureHolder {
  public:
    GraphicsComponent(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager);
    ~GraphicsComponent();
    
    void update(float deltaTime) override;
    void render() override;

    bool setTexture(Project::Handlers::ResourcesHandler& resourcesHandler, const std::string& imagePath);
    void setShape(int width, int height, SDL_Color color);
    void setPosition(int x, int y, int width, int height);
    void setEntityPosition(int x, int y) override;

    const SDL_Rect& getRect() const { return destRect; }
    int getWidth() const { return destRect.w; }
    int getHeight() const { return destRect.h; }
    
    Project::Handlers::AnimationHandler* getAnimationHandler() { return animationHandler.get(); }

  private:
    SDL_Renderer* renderer;
    Project::Utilities::LogsManager& logsManager;

    std::unique_ptr<Project::Handlers::AnimationHandler> animationHandler;

    SDL_Color shapeColor = Constants::DEFAULT_SHAPE_COLOR;
    SDL_Rect destRect{0, 0, 0, 0};

    bool drawShape = false;
  };
}
#endif
