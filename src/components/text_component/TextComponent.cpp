#include "TextComponent.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "services/styling/StyleManager.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  using Project::Utilities::ColorUtils;
  using Project::Utilities::ConfigReader;
  using Project::Handlers::Animation;
  using Project::Handlers::AnimationHandler;
  using Project::Services::StyleManager;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  TextComponent::TextComponent(SDL_Renderer* renderer, Project::Utilities::ConfigReader& configReader, LogsManager& logsManager)
  : BaseComponent(logsManager),
    animationHandler(renderer, logsManager),
    renderer(renderer),
    font(nullptr),
    configReader(configReader),
    color(Constants::DEFAULT_DEBUG_TEXT_COLOR),
    rect{0, 0, 0, 0},
    currentText(Constants::DEFAULT_TEXT),
    fontPath(Constants::DEFAULT_FONT_PATH),
    fontSize(Constants::DEFAULT_FONT_SIZE) {}

  TextComponent::~TextComponent() {
    destroyTexture();
    if (font) {
      TTF_CloseFont(font);
      font = nullptr;
    }
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

    if (borderWidth > 0) {
      SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
      for (int i = 0; i < borderWidth; ++i) {
        SDL_Rect bRect = {rect.x + i, rect.y + i, rect.w - 2 * i, rect.h - 2 * i};
        SDL_RenderDrawRect(renderer, &bRect);
      }
    }
  }

  void TextComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::FONT_COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::FONT_COLOR_ALPHA, Constants::FULL_ALPHA));
    color = Project::Utilities::ColorUtils::hexToRGB(colorHex, alpha);

    currentText = luaStateWrapper.getTableString(tableName, Keys::TEXT, Constants::DEFAULT_TEXT);

    std::string defaultFontPath = configReader.getValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_PATH, Constants::DEFAULT_FONT_PATH);
    fontPath = luaStateWrapper.getTableString(tableName, Keys::FONT_PATH, defaultFontPath);

    int defaultFontSize = configReader.getIntValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_SIZE, Constants::DEFAULT_FONT_SIZE);
    fontSize = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::FONT_SIZE, static_cast<float>(defaultFontSize)));

    if (font) {
      TTF_CloseFont(font);
    }
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (logsManager.checkAndLogError(font == nullptr, std::string("Failed to load font: ") + fontPath)) {
      return;
    }

    createTexture();
  }

  void TextComponent::applyStyle() {
    std::istringstream classes(getClass());
    std::string cls;
    while (classes >> cls) {
      std::string selector = "." + cls;
      Project::Services::Style s = Project::Services::StyleManager::getInstance().getStyle(selector);
      if (s.fontColor.a != 0) {
        color = s.fontColor;
      }

      if (s.opacity != Constants::DEFAULT_WHOLE) {
        float opacity = s.opacity;
        if (opacity > Constants::DEFAULT_WHOLE) {
          color.a = static_cast<Uint8>(std::min(opacity, static_cast<float>(Project::Libraries::Constants::FULL_ALPHA)));
        } else {
          color.a = static_cast<Uint8>(opacity * Project::Libraries::Constants::FULL_ALPHA);
        }
      }

      if (s.fontColor.a != 0 || s.opacity != Constants::DEFAULT_WHOLE) {
        createTexture();
      }

      if (s.fontSize > 0 && fontPath.size() > 0) {
        if (font) {
          TTF_CloseFont(font);
        }
        fontSize = s.fontSize;
        font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
          logsManager.logError(std::string("Failed to load font: ") + fontPath);
          return;
        }
        createTexture();
      }

      if (s.borderColor.a != 0) {
        borderColor = s.borderColor;
      }
      
      if (s.borderWidth > 0) {
        borderWidth = s.borderWidth;
      }
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
