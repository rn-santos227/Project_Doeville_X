#include "ModalComponent.h"
#include "ModalTypeResolver.h"

#include <sstream>

#include "entities/Entity.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "services/styling/StyleManager.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Components {
  using Project::Utilities::ColorUtils;
  using Project::Services::Style;
  using Project::Services::StyleManager;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  ModalComponent::ModalComponent(
    SDL_Renderer* renderer,
    Project::Utilities::LogsManager& logsManager,
    Project::Utilities::ConfigReader& configReader,
    Project::Handlers::MouseHandler* mouseHandler)
      : BaseComponent(logsManager), renderer(renderer), configReader(configReader), mouseHandler(mouseHandler) {}

  ModalComponent::~ModalComponent() {
    if (titleTexture) {
      SDL_DestroyTexture(titleTexture);
      titleTexture = nullptr;
    }
    if (subtitleTexture) {
      SDL_DestroyTexture(subtitleTexture);
      subtitleTexture = nullptr;
    }
    if (messageTexture) {
      SDL_DestroyTexture(messageTexture);
      messageTexture = nullptr;
    }
    if (okTextTexture) {
      SDL_DestroyTexture(okTextTexture);
      okTextTexture = nullptr;
    }
    if (font) {
      TTF_CloseFont(font);
      font = nullptr;
    }
  }

  void ModalComponent::update(float /*deltaTime*/) {
    if (!owner || conditionFunc.empty()) return;
    auto& lua = owner->getLuaStateWrapper();
    if (lua.callGlobalFunction(conditionFunc, 0, 1)) {
      bool result = lua_toboolean(lua.get(), -1);
      lua_pop(lua.get(), 1);
      if (!result) dismissed = false;
      setActive(result && !dismissed);
    }
    if (!isActive() || modalType != ModalType::NOTIFICATION || !mouseHandler) return;

    int mx = mouseHandler->getMouseX();
    int my = mouseHandler->getMouseY();
    SDL_Point p{mx, my};
    bool inside = SDL_PointInRect(&p, &okRect);
    bool pressed = mouseHandler->isButtonDown(SDL_BUTTON_LEFT);
    if (inside && pressed && !okWasPressed) {
      dismissed = true;
      setActive(false);
      if (!okCallback.empty()) {
        lua.callGlobalFunction(okCallback);
      }
    }
    okWasPressed = pressed;
  }

  void ModalComponent::render() {
    if (!renderer || !isActive()) return;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);

    if (borderWidth > 0) {
      SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
      for (int i = 0; i < borderWidth; ++i) {
        SDL_Rect bRect = {rect.x + i, rect.y + i, rect.w - 2 * i, rect.h - 2 * i};
        SDL_RenderDrawRect(renderer, &bRect);
      }
    }

    if (titleTexture) {
      SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
    }
    if (subtitleTexture) {
      SDL_RenderCopy(renderer, subtitleTexture, nullptr, &subtitleRect);
    }
    if (messageTexture) {
      SDL_RenderCopy(renderer, messageTexture, nullptr, &messageRect);
    }

    if (modalType == ModalType::NOTIFICATION) {
      SDL_SetRenderDrawColor(renderer, okColor.r, okColor.g, okColor.b, okColor.a);
      SDL_RenderFillRect(renderer, &okRect);

      if (okBorderWidth > 0) {
        SDL_SetRenderDrawColor(renderer, okBorderColor.r, okBorderColor.g, okBorderColor.b, okBorderColor.a);
        for (int i = 0; i < okBorderWidth; ++i) {
          SDL_Rect bRect = {okRect.x + i, okRect.y + i, okRect.w - 2 * i, okRect.h - 2 * i};
          SDL_RenderDrawRect(renderer, &bRect);
        }
      }

      if (okTextTexture) {
        SDL_RenderCopy(renderer, okTextTexture, nullptr, &okTextRect);
      }
    }
  }

  void ModalComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    int width = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::WIDTH, Constants::DEFAULT_COMPONENT_SIZE));
    int height = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::HEIGHT, Constants::DEFAULT_COMPONENT_SIZE));
    setSize(width, height);

    std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::COLOR_ALPHA, Constants::FULL_ALPHA));
    color = ColorUtils::hexToRGB(colorHex, alpha);

    title = luaStateWrapper.getTableString(tableName, Keys::TITLE, Constants::DEFAULT_TEXT);
    subtitle = luaStateWrapper.getTableString(tableName, Keys::SUBTITLE, Constants::EMPTY_STRING);
    message = luaStateWrapper.getTableString(tableName, Keys::MESSAGE, Constants::EMPTY_STRING);

    std::string defaultFontPath = configReader.getValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_PATH, Constants::DEFAULT_FONT_PATH);
    std::string fontPath = luaStateWrapper.getTableString(tableName, Keys::FONT_PATH, defaultFontPath);
    int defaultFontSize = configReader.getIntValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_SIZE, Constants::DEFAULT_FONT_SIZE);
    fontSize = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::FONT_SIZE, static_cast<float>(defaultFontSize)));
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) {
      logsManager.logWarning(std::string("Failed to load font: ") + fontPath + ". Using fallback font.");
      font = TTF_OpenFont(Constants::DEFAULT_FONT_PATH, fontSize);
      if (!font) {
        logsManager.logError(std::string("Failed to load fallback font: ") + Constants::DEFAULT_FONT_PATH);
      }
    }

    std::string fontColorHex = luaStateWrapper.getTableString(tableName, Keys::FONT_COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    fontColor = ColorUtils::hexToRGB(fontColorHex, Constants::FULL_ALPHA);

    conditionFunc = luaStateWrapper.getTableString(tableName, Keys::CONDITION, Constants::EMPTY_STRING);
    okCallback = luaStateWrapper.getTableString(tableName, Keys::OK_CALLBACK, Constants::EMPTY_STRING);
    std::string typeStr = luaStateWrapper.getTableString(tableName, Keys::TYPE, Constants::EMPTY_STRING);
    modalType = ModalTypeResolver::resolve(typeStr);

    createTitleTexture();
    createSubtitleTexture();
    createMessageTexture();
    if (modalType == ModalType::NOTIFICATION) {
      createOkTextTexture();
    }
    onAttach();
  }

  void ModalComponent::applyStyle() {
    std::istringstream classes(getClass());
    std::string cls;
    while (classes >> cls) {
      std::string selector = "." + cls;
      Style s = StyleManager::getInstance().getStyle(selector);
      if (s.width > 0) rect.w = s.width;
      if (s.height > 0) rect.h = s.height;
      if (s.background.a != 0) color = s.background;
      if (s.fontColor.a != 0) fontColor = s.fontColor;
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
        fontSize = s.fontSize;
      }
      if (s.borderColor.a != 0) borderColor = s.borderColor;
      if (s.borderWidth > 0) borderWidth = s.borderWidth;
    }

    createTitleTexture();
    createSubtitleTexture();
    createMessageTexture();
    if (modalType == ModalType::NOTIFICATION) {
      createOkTextTexture();
    }
  }

  void ModalComponent::setEntityPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
    createTitleTexture();
    createSubtitleTexture();
    createMessageTexture();
  }

  void ModalComponent::setSize(int w, int h) {
    rect.w = w;
    rect.h = h;
    createTitleTexture();
    createSubtitleTexture();
    createMessageTexture();
  }

  void ModalComponent::createTitleTexture() {
    if (!renderer || !font) return;
    if (titleTexture) {
      SDL_DestroyTexture(titleTexture);
      titleTexture = nullptr;
    }
    if (title.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, title.c_str(), fontColor);
    if (!surface) return;
    titleTexture = SDL_CreateTextureFromSurface(renderer, surface);
    titleRect = {rect.x + 10, rect.y + 10, surface->w, surface->h};
    SDL_FreeSurface(surface);
  }

  void ModalComponent::createSubtitleTexture() {
    if (!renderer || !font) return;
    if (subtitleTexture) {
      SDL_DestroyTexture(subtitleTexture);
      subtitleTexture = nullptr;
    }
    if (subtitle.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, subtitle.c_str(), fontColor);
    if (!surface) return;
    int y = rect.y + 10;
    if (titleTexture) {
      y = titleRect.y + titleRect.h + 5;
    }
    subtitleTexture = SDL_CreateTextureFromSurface(renderer, surface);
    subtitleRect = {rect.x + 10, y, surface->w, surface->h};
    SDL_FreeSurface(surface);
  }

  void ModalComponent::createMessageTexture() {
    if (!renderer || !font) return;
    if (messageTexture) {
      SDL_DestroyTexture(messageTexture);
      messageTexture = nullptr;
    }
    if (message.empty()) {
      if (modalType == ModalType::NOTIFICATION) {
        positionOkButton();
      }
      return;
    }
    SDL_Surface* surface = TTF_RenderText_Blended(font, message.c_str(), fontColor);
    if (!surface) {
      if (modalType == ModalType::NOTIFICATION) {
        positionOkButton();
      }
      return;
    }
    int y = rect.y + 10;
    if (subtitleTexture) {
      y = subtitleRect.y + subtitleRect.h + 5;
    } else if (titleTexture) {
      y = titleRect.y + titleRect.h + 5;
    }
    messageTexture = SDL_CreateTextureFromSurface(renderer, surface);
    messageRect = {rect.x + 10, y, surface->w, surface->h};
    SDL_FreeSurface(surface);
    if (modalType == ModalType::NOTIFICATION) {
      positionOkButton();
    }
  }

  void ModalComponent::createOkTextTexture() {
    if (!renderer || !font) return;
    if (okTextTexture) {
      SDL_DestroyTexture(okTextTexture);
      okTextTexture = nullptr;
    }
    SDL_Surface* surface = TTF_RenderText_Blended(font, okText.c_str(), okTextColor);
    if (!surface) return;
    okTextTexture = SDL_CreateTextureFromSurface(renderer, surface);
    okTextRect.w = surface->w;
    okTextRect.h = surface->h;
    SDL_FreeSurface(surface);
    positionOkButton();
  }

  void ModalComponent::positionOkButton() {
    okRect.x = rect.x + (rect.w - okRect.w) / 2;
    okRect.y = rect.y + rect.h - okRect.h - 10;
    okTextRect.x = okRect.x + (okRect.w - okTextRect.w) / 2;
    okTextRect.y = okRect.y + (okRect.h - okTextRect.h) / 2;
  }
}
