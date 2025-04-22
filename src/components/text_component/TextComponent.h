#ifndef TEXT_COMPONENT_H
#define TEXT_COMPONENT_H
#include "BaseComponent.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

namespace Project::Components {
  class TextComponent : public BaseComponent {
  public:
    TextComponent(SDL_Renderer* renderer, const std::string& text, const std::string& fontPath, int fontSize, SDL_Color color);
    ~TextComponent();

    void update(float deltaTime) override;
  };
}

#endif