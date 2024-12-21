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
    

  };
}

#endif