#include "TextComponent.h"
#include "libraries/constants/Constants.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  using Project::Utilities::ColorUtils;
  using Project::Handlers::Animation;
  using Project::Handlers::AnimationHandler;

  namespace Constants = Project::Libraries::Constants;

  TextComponent::TextComponent(SDL_Renderer* renderer, TTF_Font* font,  SDL_Color color, const std::string& text, const std::string& fontPath, int fontSize, LogsManager& logsManager)
  : BaseComponent(logsManager),
    animationHandler(renderer, logsManager),
    renderer(renderer), 
    font(font),
    color(color),
    rect{0, 0, 0, 0}, 
    currentText(text),
    fontPath(fontPath),
    fontSize(fontSize) {
    createTexture();
  }

  TextComponent::~TextComponent() {
    destroyTexture();
    if (font) {
      TTF_CloseFont(font);
      font = nullptr;
    }
  }

  void TextComponent::setText(const std::string& newText) {
    currentText = newText;
    createTexture();
  }

  void TextComponent::setColor(SDL_Color newColor) {
    color = newColor;
    createTexture();
  }

  void TextComponent::setColorHex(const std::string& hex) {
    color = ColorUtils::hexToRGB(hex, color.a);
    createTexture();
  }

  void TextComponent::setPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
  }

  void TextComponent::setEntityPosition(int x, int y) {
    setPosition(x, y);
  }

  void TextComponent::update(float deltaTime) {
    animationHandler.update(static_cast<Uint32>(deltaTime * Constants::MILLISECONDS_PER_SECOND));
  }

  void TextComponent::render() {
    if (animationHandler.isAnimationActive()) {
      SDL_Texture* frame = animationHandler.getCurrentFrameTexture();
      if (frame) {
        SDL_RenderCopy(renderer, frame, nullptr, &rect);
      }
    }

    if (texture) {
      SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
  }

  void TextComponent::createTexture() {
    destroyTexture();
    if (!font) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, currentText.c_str(), color);
    if (surface) {
      texture = SDL_CreateTextureFromSurface(renderer, surface);
      rect.w = surface->w;
      rect.h = surface->h;
      SDL_FreeSurface(surface);
    }
  }
}
