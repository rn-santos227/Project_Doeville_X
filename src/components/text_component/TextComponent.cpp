#include "TextComponent.h"

namespace Project::Components {
  TextComponent::TextComponent(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, const std::string& text, const std::string& fontPath, int fontSize, LogsManager& logsManager)
  : BaseComponent(logsManager), renderer(renderer), font(font), color(color), texture(nullptr), currentText(text) {
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (logsManager.checkAndLogError(!font, "Failed to load font: " + fontPath + " Error: " + std::string(TTF_GetError()))) {
      logsManager.flushLogs();
      return;
    }
  }

  TextComponent::~TextComponent() {
    freeTexture();
    if (font) {
      TTF_CloseFont(font);
      font = nullptr;
    }
  }

  void TextComponent::createTexture() {
    freeTexture();
    if (!font) return;
    
    SDL_Surface* surface = TTF_RenderText_Blended(font, currentText.c_str(), color);
    if(logsManager.checkAndLogError(!surface, "Failed to create surface for text: " + currentText + " Error: " + std::string(TTF_GetError()))) {
      logsManager.flushLogs();
      return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if(logsManager.checkAndLogError(!texture, "Failed to create texture from surface: " + currentText + " Error: " + std::string(SDL_GetError()))) {
      logsManager.flushLogs();
      return;
    }

    rect.w = surface->w;
    rect.h = surface->h;

    SDL_FreeSurface(surface);
  }

  void TextComponent::update(float /*deltaTime*/) {
    // Optional: update text based on logic
  }

  void TextComponent::render() {
    if (texture) {

    }
  }

  void TextComponent::freeTexture() {
    if (texture) {
      SDL_DestroyTexture(texture);
      texture = nullptr;
    }
  }
}
