#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include <string>
#include <SDL.h>

#include "components/BaseComponent.h"
#include "components/TextureHolder.h"

#include "handlers/animation/AnimationHandler.h"
#include "handlers/resources/ResourcesHandler.h"

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Handlers;
using namespace Project::Utilities;

namespace Project::Components  {
  class GraphicsComponent : public BaseComponent, public TextureHolder {
  public:
    GraphicsComponent(SDL_Renderer* renderer, LogsManager& logsManager);
    ~GraphicsComponent();
    
    void update(float deltaTime) override;
    void render() override;

    bool setTexture(ResourcesHandler& resourcesHandler, const std::string& imagePath);
    void setPosition(int x, int y, int width, int height);

    AnimationHandler* getAnimationHandler() { return animationHandler.get(); }

  private:
    SDL_Renderer* renderer;
    LogsManager& logsManager;

    SDL_Rect destRect{0, 0, 0, 0};
    std::unique_ptr<AnimationHandler> animationHandler;
  };
}
#endif
