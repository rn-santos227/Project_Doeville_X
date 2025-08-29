#include "ButtonComponent.h"

#include <algorithm>

#include "entities/Entity.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/color/ColorUtils.h"
#include "services/styling/StyleManager.h"
#include "utilities/geometry/GeometryUtils.h"

namespace Project::Components {
  using Project::Utilities::ColorUtils;
  using Project::Handlers::CursorHandler;
  using Project::Handlers::MouseHandler;
  using Project::Services::Style;
  using Project::Services::StyleManager;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;
  
  ButtonComponent::ButtonComponent(
    SDL_Renderer* renderer, 
    Project::Utilities::LogsManager& logsManager, Project::Utilities::ConfigReader& configReader,
    Project::Handlers::MouseHandler* mouseHandler, Project::Handlers::CursorHandler* cursorHandler)
      : BaseComponent(logsManager), renderer(renderer), configReader(configReader), mouseHandler(mouseHandler), cursorHandler(cursorHandler) {
    data.hoverColor = data.color;
    data.fontHoverColor = data.fontColor;
  }

  ButtonComponent::~ButtonComponent() {
    if (textTexture) {
      SDL_DestroyTexture(textTexture);
      textTexture = nullptr;
    }
    if (font) {
      TTF_CloseFont(font);
      font = nullptr;
    }
  }

   void ButtonComponent::update(float /*deltaTime*/) {
    if (!mouseHandler || !cursorHandler) return;

    int x = mouseHandler->getMouseX();
    int y = mouseHandler->getMouseY();
    
    SDL_Rect detectRect = {
      data.rect.x - data.marginLeft, data.rect.y - data.marginTop,
      data.rect.w + data.marginLeft + data.marginRight,
      data.rect.h + data.marginTop + data.marginBottom
    };
    
    SDL_Point mousePoint = {x, y};
    bool inside = SDL_PointInRect(&mousePoint, &detectRect);

    if (inside) {
      if (!data.hovered) {
        data.hovered = true;
        cursorHandler->setCursorState(Project::Handlers::CursorState::HOVER);
        cursorHandler->setCursorState(Project::Handlers::CursorState::HOVER);
        createTextTexture(data.fontHoverColor);
      }

      bool pressed = mouseHandler->isButtonDown(SDL_BUTTON_LEFT);
      if (pressed && !data.wasPressed) {
        if (owner && !data.luaFunction.empty()) {
          owner->callLuaFunction(data.luaFunction);
        }
      }
      data.wasPressed = pressed;
    } else {
      if (data.hovered) {
        data.hovered = false;
        cursorHandler->setCursorState(Project::Handlers::CursorState::DEFAULT);
        createTextTexture(data.fontColor);
      }
      data.wasPressed = false;
    }
  }

  void ButtonComponent::render() {
    if (!renderer) return;
    SDL_Color drawColor = data.hovered ? data.hoverColor : data.color;
    SDL_SetRenderDrawColor(renderer, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
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

    if (textTexture) {
      SDL_RenderCopy(renderer, textTexture, nullptr, &data.textRect);
    }
  }

  void ButtonComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    data.offsetX = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::X, 0.0f));
    data.offsetY = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::Y, 0.0f));
    int width = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::WIDTH, Constants::DEFAULT_COMPONENT_SIZE));
    int height = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::HEIGHT, Constants::DEFAULT_COMPONENT_SIZE));
    setSize(width, height);

    std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::COLOR_ALPHA, Constants::FULL_ALPHA));
    data.color = ColorUtils::hexToRGB(colorHex, alpha);
    data.hoverColor = data.color;

    data.text = luaStateWrapper.getTableString(tableName, Keys::TEXT, Constants::DEFAULT_TEXT);

    std::string defaultFontPath = configReader.getValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_PATH, Constants::DEFAULT_FONT_PATH);
    std::string fontPath = luaStateWrapper.getTableString(tableName, Keys::FONT_PATH, defaultFontPath);
    int defaultFontSize = configReader.getIntValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_SIZE, Constants::DEFAULT_FONT_SIZE);
    
    data.fontSize = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::FONT_SIZE, static_cast<float>(defaultFontSize)));
    font = TTF_OpenFont(fontPath.c_str(), data.fontSize);
    if (!font) {
      logsManager.logWarning(std::string("Failed to load font: ") + fontPath + ". Using fallback font.");
      font = TTF_OpenFont(Constants::DEFAULT_FONT_PATH, data.fontSize);
      if (!font) {
        logsManager.logError(std::string("Failed to load fallback font: ") + Constants::DEFAULT_FONT_PATH);
      }
    }

    std::string fontColorHex = luaStateWrapper.getTableString(tableName, Keys::FONT_COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    SDL_Color tmpColor = ColorUtils::hexToRGB(fontColorHex, Constants::FULL_ALPHA);
    data.fontColor = tmpColor;
    data.fontHoverColor = data.fontColor;

    createTextTexture(data.fontColor);
    data.luaFunction = luaStateWrapper.getTableString(tableName, Keys::CALLBACKS, Constants::EMPTY_STRING);
    onAttach();
  }

  void ButtonComponent::applyStyle() {
    std::istringstream classes(getClass());
    std::string cls;
    while (classes >> cls) {
     std::string selector = "." + cls;
      Style s = StyleManager::getInstance().getStyle(selector);
      if (s.width > 0) data.rect.w = s.width;
      if (s.height > 0) data.rect.h = s.height;
      if (s.background.a != 0) data.color = s.background;
      if (s.hoverColor.a != 0) data.hoverColor = s.hoverColor;
      if (s.fontColor.a != 0) { data.fontColor = s.fontColor; }
      if (s.fontHoverColor.a != 0) { data.fontHoverColor = s.fontHoverColor; }
      if (s.fontSize > 0) {
        if (font) TTF_CloseFont(font);
        std::string defaultFontPath = configReader.getValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_PATH, Constants::DEFAULT_FONT_PATH);
        font = TTF_OpenFont(defaultFontPath.c_str(), s.fontSize);
        if (!font) {
          logsManager.logWarning(std::string("Failed to load font: ") + defaultFontPath + ". Using fallback font.");
          font = TTF_OpenFont(Constants::DEFAULT_FONT_PATH, s.fontSize);
          if (!font) {
            logsManager.logError(std::string("Failed to load fallback font: ") + Constants::DEFAULT_FONT_PATH);
          }
        }
        data.fontSize = s.fontSize;
      }
      if (s.borderColor.a != 0) data.borderColor = s.borderColor;
      if (s.borderWidth > 0) data.borderWidth = s.borderWidth;
      if (s.borderRadius > 0) data.borderRadius = static_cast<int>(s.borderRadius);
      if (s.paddingTop || s.paddingRight || s.paddingBottom || s.paddingLeft) {
        data.paddingTop = s.paddingTop; data.paddingRight = s.paddingRight; data.paddingBottom = s.paddingBottom; data.paddingLeft = s.paddingLeft;
      }
      if (s.marginTop || s.marginRight || s.marginBottom || s.marginLeft) {
        data.marginTop = s.marginTop; data.marginRight = s.marginRight; data.marginBottom = s.marginBottom; data.marginLeft = s.marginLeft;
      }
    }
    createTextTexture(data.hovered ? data.fontHoverColor : data.fontColor);
  }

  void ButtonComponent::setEntityPosition(float x, float y) {
    data.rect.x = x + data.offsetX;
    data.rect.y = y + data.offsetY;
    createTextTexture(data.hovered ? data.fontHoverColor : data.fontColor);
  }

  void ButtonComponent::setSize(int w, int h) {
    data.rect.w = w;
    data.rect.h = h;
    createTextTexture(data.hovered ? data.fontHoverColor : data.fontColor);
  }
  
  void ButtonComponent::setFontColor(SDL_Color _color) {
    data.fontColor = _color; createTextTexture(data.hovered ? data.fontHoverColor : data.fontColor);
  }
  
  void ButtonComponent::setFontHoverColor(SDL_Color _color) {
    data.fontHoverColor = _color; createTextTexture(data.hovered ? data.fontHoverColor : data.fontColor);
  }

  void ButtonComponent::createTextTexture(SDL_Color colorToUse) {
    if (!font || data.text.empty()) return;
    if (textTexture) {
      SDL_DestroyTexture(textTexture);
      textTexture = nullptr;
    }
    SDL_Surface* surface = TTF_RenderText_Blended(font, data.text.c_str(), colorToUse);

    if (!surface) {
      logsManager.logWarning(std::string("Failed to render button text: ") + TTF_GetError());
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

    textTexture = SDL_CreateTextureFromSurface(renderer, surface);
    data.textRect.w = surface->w;
    data.textRect.h = surface->h;
    SDL_FreeSurface(surface);
    if (!textTexture) {
      logsManager.logWarning(std::string("Failed to create button text texture: ") + SDL_GetError());
    }
    data.textRect.x = data.rect.x + data.paddingLeft + (data.rect.w - data.paddingLeft - data.paddingRight - data.textRect.w) / Project::Libraries::Constants::INDEX_TWO;
    data.textRect.y = data.rect.y + data.paddingTop + (data.rect.h - data.paddingTop - data.paddingBottom - data.textRect.h) / Project::Libraries::Constants::INDEX_TWO;
  }
}
