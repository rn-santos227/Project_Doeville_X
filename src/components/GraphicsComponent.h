#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include "BaseComponent.h"

#include <string>

#include <SDL.h>

namespace Project::Components  {
  class GraphicsComponent : public BaseComponent {
  public:
    explicit GraphicsComponent(SDL_Renderer* renderer, const std::string& texturePath);
    ~GraphicsComponent();
    
    void update(float deltaTime) override;
    void render() override;

    bool setTexture(const std::string& texturePath);

  private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect destinationRect;

    void loadTexture(const std::string& path);
  };
}

#endif