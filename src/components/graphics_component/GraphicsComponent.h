#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include <string>
#include <SDL.h>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/TextureHolder.h"


#include "handlers/animation/AnimationHandler.h"
#include "handlers/resources/ResourcesHandler.h"

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Handlers;
using namespace Project::Utilities;

namespace Project::Components  {
  class GraphicsComponent : public BaseComponent, public PositionableComponent, public TextureHolder {
  public:
    GraphicsComponent(SDL_Renderer* renderer, LogsManager& logsManager);
    ~GraphicsComponent();
    
    void update(float deltaTime) override;
    void render() override;

    bool setTexture(ResourcesHandler& resourcesHandler, const std::string& imagePath);
    void setShape(int width, int height, SDL_Color color);
    void setPosition(int x, int y, int width, int height);
    void setEntityPosition(int x, int y) override;

    const SDL_Rect& getRect() const { return destRect; }
    int getWidth() const { return destRect.w; }
    int getHeight() const { return destRect.h; }
    

    AnimationHandler* getAnimationHandler() { return animationHandler.get(); }

  private:
    SDL_Renderer* renderer;
    LogsManager& logsManager;

    std::unique_ptr<AnimationHandler> animationHandler;

    SDL_Color shapeColor{255, 255, 255, 255};
    SDL_Rect destRect{0, 0, 0, 0};

    bool drawShape = false;
  };
}
#endif
