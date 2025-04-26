#include "TextComponent.h"

namespace Project::Components {
  TextComponent::TextComponent(SDL_Renderer* renderer, const std::string& text, TTF_Font* font, SDL_Color color, LogsManager& logsManager)
  : BaseComponent(logsManager), renderer(renderer), font(font), color(color), texture(nullptr), currentText(text) {

  }

  void TextComponent::freeTexture() {
    if (texture) {

    }
  }
}
