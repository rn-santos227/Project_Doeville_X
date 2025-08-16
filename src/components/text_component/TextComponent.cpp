#include <algorithm>

#include "TextComponent.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "services/styling/StyleManager.h"
#include "utilities/color/ColorUtils.h"
#include "utilities/geometry/GeometryUtils.h"

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
    configReader(configReader) {
      data.textColor = Constants::DEFAULT_DEBUG_TEXT_COLOR;
      data.rect = {0, 0, 0, 0};
      data.currentText = Constants::DEFAULT_TEXT;
      data.fontPath = Constants::DEFAULT_FONT_PATH;
      data.fontSize = Constants::DEFAULT_FONT_SIZE;     
    }

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
    if (data.bgColor.a > 0) {
      SDL_SetRenderDrawColor(renderer, data.bgColor.r, data.bgColor.g, data.bgColor.b, data.bgColor.a);
      if (data.borderRadius > 0) {
        SDL_FRect rectF{
          static_cast<float>(data.rect.x),
          static_cast<float>(data.rect.y),
          static_cast<float>(data.rect.w),
          static_cast<float>(data.rect.h)
        };
        Project::Utilities::GeometryUtils::renderFilledRoundedRect(renderer, rectF, data.borderRadius);
      } else {
        SDL_RenderFillRect(renderer, &data.rect);
      }
    }

    if (animationHandler.isAnimationActive()) {
      SDL_Texture* frame = animationHandler.getCurrentFrameTexture();
      if (frame) {
        SDL_RenderCopy(renderer, frame, nullptr, &data.rect);
      }
    }

    if (texture) {
      SDL_RenderCopy(renderer, texture, nullptr, &data.textRect);
    }

    if (data.borderWidth > 0) {
      SDL_SetRenderDrawColor(renderer, data.borderColor.r, data.borderColor.g, data.borderColor.b, data.borderColor.a);
      for (int i = 0; i < data.borderWidth; ++i) {
        SDL_Rect bRect = {data.rect.x + i, data.rect.y + i, data.rect.w - Constants::INDEX_TWO * i, data.rect.h - Constants::INDEX_TWO * i};
        if (data.borderRadius > 0) {
          int radius = std::max(0, data.borderRadius - i);
          SDL_FRect bRectF{
            static_cast<float>(bRect.x),
            static_cast<float>(bRect.y),
            static_cast<float>(bRect.w),
            static_cast<float>(bRect.h)
          };
          Project::Utilities::GeometryUtils::renderRoundedRect(renderer, bRectF, radius);
        } else {
          SDL_RenderDrawRect(renderer, &bRect);
        }
      }
    }
  }

  void TextComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    data.offsetX = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::X, 0.0f));
    data.offsetY = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::Y, 0.0f));
    std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::FONT_COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::FONT_COLOR_ALPHA, Constants::FULL_ALPHA));
    data.textColor = Project::Utilities::ColorUtils::hexToRGB(colorHex, alpha);

    data.currentText = luaStateWrapper.getTableString(tableName, Keys::TEXT, Constants::DEFAULT_TEXT);

    std::string defaultFontPath = configReader.getValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_PATH, Constants::DEFAULT_FONT_PATH);
    data.fontPath = luaStateWrapper.getTableString(tableName, Keys::FONT_PATH, defaultFontPath);

    int defaultFontSize = configReader.getIntValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_SIZE, Constants::DEFAULT_FONT_SIZE);
    data.fontSize = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::FONT_SIZE, static_cast<float>(defaultFontSize)));

    if (font) {
      TTF_CloseFont(font);
    }

    font = TTF_OpenFont(data.fontPath.c_str(), data.fontSize);
    if (!font) {
      logsManager.logWarning(std::string("Failed to load font: ") + data.fontPath + ". Using fallback font.");
      font = TTF_OpenFont(Constants::DEFAULT_FONT_PATH, data.fontSize);
      if (!font) {
        logsManager.logError(std::string("Failed to load fallback font: ") + Constants::DEFAULT_FONT_PATH);
        return;
      }
    }

    createTexture();
  }

  void TextComponent::applyStyle() {
    std::istringstream classes(getClass());
    std::string cls;
    bool needsTexture = false;
    
    while (classes >> cls) {
      std::string selector = "." + cls;
      Project::Services::Style s = Project::Services::StyleManager::getInstance().getStyle(selector);
      if (s.fontColor.a != 0) {
        data.textColor = s.fontColor;
        needsTexture = true;
      }

      if (s.opacity != Constants::DEFAULT_WHOLE) {
        float opacity = s.opacity;
        if (opacity > Constants::DEFAULT_WHOLE) {
          data.textColor.a = static_cast<Uint8>(std::min(opacity, static_cast<float>(Project::Libraries::Constants::FULL_ALPHA)));
        } else {
          data.textColor.a = static_cast<Uint8>(opacity * Project::Libraries::Constants::FULL_ALPHA);
        }
        needsTexture = true;
      }

      if (s.fontSize > 0 && data.fontPath.size() > 0) {
        if (font) {
          TTF_CloseFont(font);
        }
        data.fontSize = s.fontSize;
        font = TTF_OpenFont(data.fontPath.c_str(), data.fontSize);
        if (!font) {
          logsManager.logWarning(std::string("Failed to load font: ") + data.fontPath + ". Using fallback font.");
          font = TTF_OpenFont(Constants::DEFAULT_FONT_PATH, data.fontSize);
          if (!font) {
            logsManager.logError(std::string("Failed to load fallback font: ") + Constants::DEFAULT_FONT_PATH);
            return;
          }
        }
        needsTexture = true;
      }

      if (s.borderColor.a != 0) {
        data.borderColor = s.borderColor;
      }
      
      if (s.borderWidth > 0) {
        data.borderWidth = s.borderWidth;
      }

      if (s.background.a != 0) {
        data.bgColor = s.background;
      }

      if (s.borderRadius > 0) {
        data.borderRadius = static_cast<int>(s.borderRadius);
      }

      if (s.paddingTop || s.paddingRight || s.paddingBottom || s.paddingLeft) {
        data.paddingTop = s.paddingTop;
        data.paddingRight = s.paddingRight;
        data.paddingBottom = s.paddingBottom;
        data.paddingLeft = s.paddingLeft;
        needsTexture = true;
      }
    }
    if (needsTexture) {
      createTexture();
    }
  }

  void TextComponent::setText(const std::string& newText) {
    data.currentText = newText;
    createTexture();
  }

  void TextComponent::setColor(SDL_Color newColor) {
    data.textColor = newColor;
    createTexture();
  }

  void TextComponent::setColorHex(const std::string& hex) {
    data.textColor = ColorUtils::hexToRGB(hex, data.textColor.a);
    createTexture();
  }

  void TextComponent::setPosition(int x, int y) {
    data.rect.x = x;
    data.rect.y = y;
    data.textRect.x = x + data.paddingLeft;
    data.textRect.y = y + data.paddingTop;
  }

  void TextComponent::setEntityPosition(float x, float y) {
    setPosition(static_cast<int>(x) + data.offsetX, static_cast<int>(y) + data.offsetY);
  }

  void TextComponent::createTexture() {
    destroyTexture();
    if (!font) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, data.currentText.c_str(), data.textColor);
    if (!surface) {
      logsManager.logWarning(std::string("Failed to render text: ") + TTF_GetError());
      surface = SDL_CreateRGBSurfaceWithFormat(0, Constants::INDEX_TWO, Constants::INDEX_TWO, Constants::BIT_32, SDL_PIXELFORMAT_RGBA32);
      if (surface) {
        Uint32 magenta = SDL_MapRGBA(surface->format, Constants::BIT_255, 0, Constants::BIT_255, Constants::BIT_255);
        Uint32 black = SDL_MapRGBA(surface->format, 0, 0, 0, Constants::BIT_255);
        Uint32* pixels = static_cast<Uint32*>(surface->pixels);
        pixels[Constants::INDEX_ZERO] = magenta;
        pixels[Constants::INDEX_ONE] = black;
        pixels[Constants::INDEX_TWO] = black;
        pixels[Constants::INDEX_THREE] = magenta;
      }
    }
    
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    data.textRect.w = surface->w;
    data.textRect.h = surface->h;
    data.rect.w = data.textRect.w + data.paddingLeft + data.paddingRight;
    data.rect.h = data.textRect.h + data.paddingTop + data.paddingBottom;
    data.textRect.x = data.rect.x + data.paddingLeft;
    data.textRect.y = data.rect.y + data.paddingTop;
    SDL_FreeSurface(surface);
    if (!texture) {
      logsManager.logWarning(std::string("Failed to create texture from text: ") + SDL_GetError());
    }
  }
}
