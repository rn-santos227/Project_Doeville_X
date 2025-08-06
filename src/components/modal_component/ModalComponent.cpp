#include "ModalComponent.h"
#include "ModalTypeResolver.h"

#include <algorithm>
#include <sstream>

#include "entities/Entity.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "services/styling/StyleManager.h"
#include "utilities/color/ColorUtils.h"
#include "utilities/geometry/GeometryUtils.h"

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
    if (data.titleTexture) {
      SDL_DestroyTexture(data.titleTexture);
      data.titleTexture = nullptr;
    }
    
    if (data.subtitleTexture) {
      SDL_DestroyTexture(data.subtitleTexture);
      data.subtitleTexture = nullptr;
    }
    
    if (data.messageTexture) {
      SDL_DestroyTexture(data.messageTexture);
      data.messageTexture = nullptr;
    
    }
    if (data.okTextTexture) {
      SDL_DestroyTexture(data.okTextTexture);
      data.okTextTexture = nullptr;
    }

    if (data.cancelTextTexture) {
      SDL_DestroyTexture(data.cancelTextTexture);
      data.cancelTextTexture = nullptr;
    
    }
    if (data.font) {
      TTF_CloseFont(data.font);
      data.font = nullptr;
    }
  }

  void ModalComponent::update(float /*deltaTime*/) {
    if (!owner || data.conditionFunc.empty()) return;
    auto& lua = owner->getLuaStateWrapper();
    if (lua.callGlobalFunction(data.conditionFunc, 0, 1)) {
      bool result = lua_toboolean(lua.get(), -1);
      lua_pop(lua.get(), 1);
      if (!result) data.dismissed = false;
      setActive(result && !data.dismissed);
    }
    if (!isActive() || data.modalType != ModalType::NOTIFICATION || !mouseHandler) return;

    int mx = mouseHandler->getMouseX();
    int my = mouseHandler->getMouseY();
    SDL_Point p{mx, my};
    bool inside = SDL_PointInRect(&p, &data.okRect);
    bool pressed = mouseHandler->isButtonDown(SDL_BUTTON_LEFT);
    if (inside && pressed && !data.okWasPressed) {
      data.dismissed = true;
      setActive(false);
      if (!data.okCallback.empty()) {
        lua.callGlobalFunction(data.okCallback);
      }
    }
    data.okWasPressed = pressed;
  }

  void ModalComponent::render() {
    if (!renderer || !isActive()) return;
    SDL_SetRenderDrawColor(renderer, data.color.r, data.color.g, data.color.b, data.color.a);
    SDL_RenderFillRect(renderer, &data.rect);

    if (data.borderWidth > 0) {
      SDL_SetRenderDrawColor(renderer, data.borderColor.r, data.borderColor.g, data.borderColor.b, data.borderColor.a);
      for (int i = 0; i < data.borderWidth; ++i) {
        SDL_Rect bRect = {data.rect.x + i, data.rect.y + i, data.rect.w - 2 * i, data.rect.h - 2 * i};
        SDL_RenderDrawRect(renderer, &bRect);
      }
    }

    if (data.titleTexture) {
      SDL_RenderCopy(renderer, data.titleTexture, nullptr, &data.titleRect);
    }
    if (data.subtitleTexture) {
      SDL_RenderCopy(renderer, data.subtitleTexture, nullptr, &data.subtitleRect);
    }
    if (data.messageTexture) {
      SDL_RenderCopy(renderer, data.messageTexture, nullptr, &data.messageRect);
    }

    if (data.modalType == ModalType::NOTIFICATION) {
      SDL_SetRenderDrawColor(renderer, data.okColor.r, data.okColor.g, data.okColor.b, data.okColor.a);
      SDL_RenderFillRect(renderer, &data.okRect);

      if (data.okBorderWidth > 0) {
        SDL_SetRenderDrawColor(renderer, data.okBorderColor.r, data.okBorderColor.g, data.okBorderColor.b, data.okBorderColor.a);
        for (int i = 0; i < data.okBorderWidth; ++i) {
          SDL_Rect bRect = {data.okRect.x + i, data.okRect.y + i, data.okRect.w - 2 * i, data.okRect.h - 2 * i};
          SDL_RenderDrawRect(renderer, &bRect);
        }
      }

      if (data.okTextTexture) {
        SDL_RenderCopy(renderer, data.okTextTexture, nullptr, &data.okTextRect);
      }
    }
  }

  void ModalComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    int width = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::WIDTH, Constants::DEFAULT_COMPONENT_SIZE));
    int height = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::HEIGHT, Constants::DEFAULT_COMPONENT_SIZE));
    setSize(width, height);

    std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::COLOR_ALPHA, Constants::FULL_ALPHA));
    data.color = ColorUtils::hexToRGB(colorHex, alpha);

    data.title = luaStateWrapper.getTableString(tableName, Keys::TITLE, Constants::DEFAULT_TEXT);
    data.subtitle = luaStateWrapper.getTableString(tableName, Keys::SUBTITLE, Constants::EMPTY_STRING);
    data.message = luaStateWrapper.getTableString(tableName, Keys::MESSAGE, Constants::EMPTY_STRING);

    std::string defaultFontPath = configReader.getValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_PATH, Constants::DEFAULT_FONT_PATH);
    std::string fontPath = luaStateWrapper.getTableString(tableName, Keys::FONT_PATH, defaultFontPath);
    int defaultFontSize = configReader.getIntValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_SIZE, Constants::DEFAULT_FONT_SIZE);
    data.fontSize = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::FONT_SIZE, static_cast<float>(defaultFontSize)));
    data.font = TTF_OpenFont(fontPath.c_str(), data.fontSize);
    if (!data.font) {
      logsManager.logWarning(std::string("Failed to load font: ") + fontPath + ". Using fallback font.");
      data.font = TTF_OpenFont(Constants::DEFAULT_FONT_PATH, data.fontSize);
      if (!data.font) {
        logsManager.logError(std::string("Failed to load fallback font: ") + Constants::DEFAULT_FONT_PATH);
      }
    }

    std::string fontColorHex = luaStateWrapper.getTableString(tableName, Keys::FONT_COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    data.fontColor = ColorUtils::hexToRGB(fontColorHex, Constants::FULL_ALPHA);

    data.conditionFunc = luaStateWrapper.getTableString(tableName, Keys::CONDITION, Constants::EMPTY_STRING);
    data.okCallback = luaStateWrapper.getTableString(tableName, Keys::OK_CALLBACK, Constants::EMPTY_STRING);
    std::string typeStr = luaStateWrapper.getTableString(tableName, Keys::TYPE, Constants::EMPTY_STRING);
    data.modalType = ModalTypeResolver::resolve(typeStr);

    createTitleTexture();
    createSubtitleTexture();
    createMessageTexture();
    if (data.modalType == ModalType::NOTIFICATION) {
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
      if (s.width > 0) data.rect.w = s.width;
      if (s.height > 0) data.rect.h = s.height;
      if (s.background.a != 0) data.color = s.background;
      if (s.fontColor.a != 0) data.fontColor = s.fontColor;
      if (s.fontSize > 0) {
        if (data.font) TTF_CloseFont(data.font);
        std::string defaultFontPath = configReader.getValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_PATH, Constants::DEFAULT_FONT_PATH);
        data.font = TTF_OpenFont(defaultFontPath.c_str(), s.fontSize);
        if (!data.font) {
          logsManager.logWarning(std::string("Failed to load font: ") + defaultFontPath + ". Using fallback font.");
          data.font = TTF_OpenFont(Constants::DEFAULT_FONT_PATH, s.fontSize);
          if (!data.font) {
            logsManager.logError(std::string("Failed to load fallback font: ") + Constants::DEFAULT_FONT_PATH);
          }
        }
        data.fontSize = s.fontSize;
      }
      if (s.borderColor.a != 0) data.borderColor = s.borderColor;
      if (s.borderWidth > 0) data.borderWidth = s.borderWidth;
    }

    createTitleTexture();
    createSubtitleTexture();
    createMessageTexture();
    if (data.modalType == ModalType::NOTIFICATION) {
      createOkTextTexture();
    }
  }

  void ModalComponent::setEntityPosition(int x, int y) {
    data.rect.x = x;
    data.rect.y = y;
    createTitleTexture();
    createSubtitleTexture();
    createMessageTexture();
  }

  void ModalComponent::setSize(int w, int h) {
    data.rect.w = w;
    data.rect.h = h;
    createTitleTexture();
    createSubtitleTexture();
  }

  void ModalComponent::createTitleTexture() {
    if (!renderer || !data.font) return;
    if (data.titleTexture) {
      SDL_DestroyTexture(data.titleTexture);
      data.titleTexture = nullptr;
    }
    if (data.title.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(data.font, data.title.c_str(), data.fontColor);
    if (!surface) return;
    data.titleTexture = SDL_CreateTextureFromSurface(renderer, surface);
    data.titleRect = {data.rect.x + Constants::DEFAULT_TEXT_MARGIN, data.rect.y + Constants::DEFAULT_TEXT_MARGIN, surface->w, surface->h};
    SDL_FreeSurface(surface);
  }

  void ModalComponent::createSubtitleTexture() {
    if (!renderer || !data.font) return;
    if (data.subtitleTexture) {
      SDL_DestroyTexture(data.subtitleTexture);
      data.subtitleTexture = nullptr;
    }
    if (data.subtitle.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(data.font, data.subtitle.c_str(), data.fontColor);
    if (!surface) return;
    int y = data.rect.y + Constants::DEFAULT_TEXT_HEIGHT_OFFSET;
    if (data.titleTexture) {
      y = data.titleRect.y + data.titleRect.h + Constants::DEFAULT_TEXT_MARGIN;
    }
    data.subtitleTexture = SDL_CreateTextureFromSurface(renderer, surface);
    data.subtitleRect = {data.rect.x + Constants::DEFAULT_TEXT_MARGIN, y, surface->w, surface->h};
    SDL_FreeSurface(surface);
  }

  void ModalComponent::createMessageTexture() {
    if (!renderer || !data.font) return;
    if (data.messageTexture) {
      SDL_DestroyTexture(data.messageTexture);
      data.messageTexture = nullptr;
    }
    if (data.message.empty()) {
      if (data.modalType == ModalType::NOTIFICATION) {
        positionOkButton();
      }
      return;
    }
    SDL_Surface* surface = TTF_RenderText_Blended(data.font, data.message.c_str(), data.fontColor);
    if (!surface) {
      if (data.modalType == ModalType::NOTIFICATION) {
        positionOkButton();
      }
      return;
    }
    int y = data.rect.y + Constants::DEFAULT_TEXT_MARGIN;
    if (data.subtitleTexture) {
      y = data.subtitleRect.y + data.subtitleRect.h + Constants::DEFAULT_TEXT_HEIGHT_OFFSET;
    } else if (data.titleTexture) {
      y = data.titleRect.y + data.titleRect.h + Constants::DEFAULT_TEXT_HEIGHT_OFFSET;
    }
    data.messageTexture = SDL_CreateTextureFromSurface(renderer, surface);
    data.messageRect = {data.rect.x + Constants::DEFAULT_TEXT_MARGIN, y, surface->w, surface->h};
    SDL_FreeSurface(surface);
    if (data.modalType == ModalType::NOTIFICATION) {
      positionOkButton();
    }
  }

  void ModalComponent::createOkTextTexture() {
    if (!renderer || !data.font) return;
    if (data.okTextTexture) {
      SDL_DestroyTexture(data.okTextTexture);
      data.okTextTexture = nullptr;
    }
    SDL_Surface* surface = TTF_RenderText_Blended(data.font, data.okText.c_str(), data.okTextColor);
    if (!surface) return;
    data.okTextTexture = SDL_CreateTextureFromSurface(renderer, surface);
    data.okTextRect.w = surface->w;
    data.okTextRect.h = surface->h;
    SDL_FreeSurface(surface);
    positionOkButton();
  }

  void ModalComponent::positionOkButton() {
    data.okRect.x = data.rect.x + (data.rect.w - data.okRect.w) / Constants::INDEX_TWO;
    data.okRect.y = data.rect.y + data.rect.h - data.okRect.h - Constants::DEFAULT_TEXT_MARGIN;
    data.okTextRect.x = data.okRect.x + (data.okRect.w - data.okTextRect.w) / Constants::INDEX_TWO;
    data.okTextRect.y = data.okRect.y + (data.okRect.h - data.okTextRect.h) / Constants::INDEX_TWO;
  }
}
