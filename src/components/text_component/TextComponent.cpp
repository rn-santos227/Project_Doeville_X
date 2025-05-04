#include "TextComponent.h"

namespace Project::Components {
  TextComponent::TextComponent(SDL_Renderer* renderer, TTF_Font* font,  SDL_Color color, const std::string& text, const std::string& fontPath, int fontSize, LogsManager& logsManager)
  : BaseComponent(logsManager), renderer(renderer), font(font), color(color), currentText(text), fontPath(fontPath), animationHandler(renderer, logsManager) {
    createTexture();
  }

  TextComponent::~TextComponent() {
    freeTexture();
    if (font) {
      TTF_CloseFont(font);
      font = nullptr;
    }
  }

  void TextComponent::setText(const std::string& newText) {
    currentText = newText;
    createTexture();
  }

  void TextComponent::setPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
  }

  void TextComponent::update(float deltaTime) {
    animationHandler.update(static_cast<Uint32>(deltaTime * 1000));
  }

  void TextComponent::render() {
    if (animationHandler.isAnimationActive()) {

    }
    
    if (texture) {
      SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
  }

  void TextComponent::createTexture() {
    freeTexture();
    if (!font) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, currentText.c_str(), color);
    if (surface) {
      texture = SDL_CreateTextureFromSurface(renderer, surface);
      rect.w = surface->w;
      rect.h = surface->h;
      SDL_FreeSurface(surface);
    }
  }

  void TextComponent::freeTexture() {
    if (texture) {
      SDL_DestroyTexture(texture);
      texture = nullptr;
    }
  }
}
