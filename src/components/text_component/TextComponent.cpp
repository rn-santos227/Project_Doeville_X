#include "TextComponent.h"

namespace Project::Components {
  TextComponent::TextComponent(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, const std::string& text, const std::string& fontPath, int fontSize, LogsManager& logsManager)
  : BaseComponent(logsManager), renderer(renderer), font(font), color(color), texture(nullptr), currentText(text) {
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
  }

  TextComponent::~TextComponent() {
    freeTexture();
    if (font) {
      TTF_CloseFont(font);
      font = nullptr;
    }
  }

  void TextComponent::freeTexture() {
    if (texture) {
      SDL_DestroyTexture(texture);
      texture = nullptr;
    }
  }
}
