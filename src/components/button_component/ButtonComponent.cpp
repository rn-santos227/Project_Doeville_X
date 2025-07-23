#include "ButtonComponent.h"

#include "entities/Entity.h"

#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Components {
  using Project::Utilities::ColorUtils;
  using Project::Handlers::CursorHandler;
  using Project::Handlers::MouseHandler;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;
  
  ButtonComponent::ButtonComponent(
    SDL_Renderer* renderer, 
    Project::Utilities::LogsManager& logsManager, Project::Utilities::ConfigReader& configReader,
    Project::Handlers::MouseHandler* mouseHandler, Project::Handlers::CursorHandler* cursorHandler)
      : BaseComponent(logsManager), renderer(renderer), configReader(configReader), mouseHandler(mouseHandler), cursorHandler(cursorHandler) {}

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
      rect.x - marginLeft, rect.y - marginTop,
      rect.w + marginLeft + marginRight,
      rect.h + marginTop + marginBottom
    };
    
    SDL_Point mousePoint = {x, y};
    bool inside = SDL_PointInRect(&mousePoint, &detectRect);

    if (inside) {
      if (!hovered) {
        hovered = true;
        cursorHandler->setCursorState(Project::Handlers::CursorState::HOVER);
        createTextTexture(fontHoverColor);
      }

      bool pressed = mouseHandler->isButtonDown(SDL_BUTTON_LEFT);
      if (pressed && !wasPressed) {
        if (owner && !luaFunction.empty()) {
          owner->callLuaFunction(luaFunction);
        }
      }
      wasPressed = pressed;
    } else {
      if (hovered) {
        hovered = false;
        cursorHandler->setCursorState(Project::Handlers::CursorState::DEFAULT);
        createTextTexture(fontColor);
      }
      wasPressed = false;
    }
  }

  void ButtonComponent::render() {
    if (!renderer) return;
    SDL_Color drawColor = hovered ? hoverColor : color;
    SDL_SetRenderDrawColor(renderer, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
    SDL_RenderFillRect(renderer, &rect);
  }

  void ButtonComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    int width = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::WIDTH, Constants::DEFAULT_COMPONENT_SIZE));
    int height = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::HEIGHT, Constants::DEFAULT_COMPONENT_SIZE));
    setSize(width, height);

    std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::COLOR_ALPHA, Constants::FULL_ALPHA));
    color = ColorUtils::hexToRGB(colorHex, alpha);
    hoverColor = color;

    text = luaStateWrapper.getTableString(tableName, Keys::TEXT, Constants::DEFAULT_TEXT);

    std::string defaultFontPath = configReader.getValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_PATH, Constants::DEFAULT_FONT_PATH);
    std::string fontPath = luaStateWrapper.getTableString(tableName, Keys::FONT_PATH, defaultFontPath);
    int defaultFontSize = configReader.getIntValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_SIZE, Constants::DEFAULT_FONT_SIZE);
    fontSize = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::FONT_SIZE, static_cast<float>(defaultFontSize)));
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) {
      logsManager.logError(std::string("Failed to load font: ") + fontPath);
    }

    std::string fontColorHex = luaStateWrapper.getTableString(tableName, Keys::FONT_COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    SDL_Color tmpColor = ColorUtils::hexToRGB(fontColorHex, Constants::FULL_ALPHA);
    fontColor = tmpColor;
    fontHoverColor = fontColor;
  }

  void ButtonComponent::createTextTexture(SDL_Color colorToUse) {
    if (!font || text.empty()) return;
    if (textTexture) {
      SDL_DestroyTexture(textTexture);
      textTexture = nullptr;
    }
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), colorToUse);
    if (surface) {
      textTexture = SDL_CreateTextureFromSurface(renderer, surface);
      textRect.w = surface->w;
      textRect.h = surface->h;
      SDL_FreeSurface(surface);
      textRect.x = rect.x + paddingLeft + (rect.w - paddingLeft - paddingRight - textRect.w) / 2;
      textRect.y = rect.y + paddingTop + (rect.h - paddingTop - paddingBottom - textRect.h) / 2;
    }
  }
}
