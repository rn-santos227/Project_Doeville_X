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
  using Project::Utilities::GeometryUtils;
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
    if (!isActive() || (data.modalType != ModalType::NOTIFICATION && data.modalType != ModalType::QUESTION) || !mouseHandler) return;

    int mx = mouseHandler->getMouseX();
    int my = mouseHandler->getMouseY();
    SDL_Point p{mx, my};
    bool pressed = mouseHandler->isButtonDown(SDL_BUTTON_LEFT);
    bool insideOk = SDL_PointInRect(&p, &data.okRect);
    bool insideCancel = SDL_PointInRect(&p, &data.cancelRect);
    if (insideOk && pressed && !data.okWasPressed) {
      data.dismissed = true;
      setActive(false);
      if (!data.okCallback.empty()) {
        lua.callGlobalFunction(data.okCallback);
      }
    }
    if (data.modalType == ModalType::QUESTION && insideCancel && pressed && !data.cancelWasPressed) {
      data.dismissed = true;
      setActive(false);
      if (!data.cancelCallback.empty()) {
        lua.callGlobalFunction(data.cancelCallback);
      }
    }
    data.okWasPressed = insideOk && pressed;
    data.cancelWasPressed = insideCancel && pressed;
  }

  void ModalComponent::render() {
    if (!renderer || !isActive()) return;
    SDL_SetRenderDrawColor(renderer, data.color.r, data.color.g, data.color.b, data.color.a);
    if (data.borderRadius > 0) {
      SDL_FRect rectF{
        static_cast<float>(data.rect.x),
        static_cast<float>(data.rect.y),
        static_cast<float>(data.rect.w),
        static_cast<float>(data.rect.h)
      };
      
      GeometryUtils::renderFilledRoundedRect(renderer, rectF, data.borderRadius);
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
          
          GeometryUtils::renderRoundedRect(renderer, bRectF, radius);
        } else {
          SDL_RenderDrawRect(renderer, &bRect);
        }
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

    if (data.modalType == ModalType::NOTIFICATION || data.modalType == ModalType::QUESTION) {
      SDL_SetRenderDrawColor(renderer, data.okColor.r, data.okColor.g, data.okColor.b, data.okColor.a);
      if (data.okBorderRadius > 0) {
        SDL_FRect okF{
          static_cast<float>(data.okRect.x),
          static_cast<float>(data.okRect.y),
          static_cast<float>(data.okRect.w),
          static_cast<float>(data.okRect.h)
        };

        GeometryUtils::renderFilledRoundedRect(renderer, okF, data.okBorderRadius);
      } else {
        SDL_RenderFillRect(renderer, &data.okRect);
      }

      if (data.okBorderWidth > 0) {
        SDL_SetRenderDrawColor(renderer, data.okBorderColor.r, data.okBorderColor.g, data.okBorderColor.b, data.okBorderColor.a);
        for (int i = 0; i < data.okBorderWidth; ++i) {
          SDL_Rect bRect = {data.okRect.x + i, data.okRect.y + i, data.okRect.w - 2 * i, data.okRect.h - 2 * i};
          if (data.okBorderRadius > 0) {
            int radius = std::max(0, data.okBorderRadius - i);
            SDL_FRect bRectF{
              static_cast<float>(bRect.x),
              static_cast<float>(bRect.y),
              static_cast<float>(bRect.w),
              static_cast<float>(bRect.h)
            };

            GeometryUtils::renderRoundedRect(renderer, bRectF, radius);
          } else {
            SDL_RenderDrawRect(renderer, &bRect);
          }
        }
      }

      if (data.okTextTexture) {
        SDL_RenderCopy(renderer, data.okTextTexture, nullptr, &data.okTextRect);
      }

      if (data.modalType == ModalType::QUESTION) {
        SDL_SetRenderDrawColor(renderer, data.cancelColor.r, data.cancelColor.g, data.cancelColor.b, data.cancelColor.a);
        if (data.cancelBorderRadius > 0) {
          SDL_FRect cancelF{
            static_cast<float>(data.cancelRect.x),
            static_cast<float>(data.cancelRect.y),
            static_cast<float>(data.cancelRect.w),
            static_cast<float>(data.cancelRect.h)
          };
          
          GeometryUtils::renderFilledRoundedRect(renderer, cancelF, data.cancelBorderRadius);
        } else {
          SDL_RenderFillRect(renderer, &data.cancelRect);
        }
        if (data.cancelBorderWidth > 0) {
          SDL_SetRenderDrawColor(renderer, data.cancelBorderColor.r, data.cancelBorderColor.g, data.cancelBorderColor.b, data.cancelBorderColor.a);
          for (int i = 0; i < data.cancelBorderWidth; ++i) {
            SDL_Rect bRect = {data.cancelRect.x + i, data.cancelRect.y + i, data.cancelRect.w - Constants::INDEX_TWO * i, data.cancelRect.h - Constants::INDEX_TWO * i};
            if (data.cancelBorderRadius > 0) {
              int radius = std::max(0, data.cancelBorderRadius - i);
              SDL_FRect bRectF{
                static_cast<float>(bRect.x),
                static_cast<float>(bRect.y),
                static_cast<float>(bRect.w),
                static_cast<float>(bRect.h)
              };
              
              GeometryUtils::renderRoundedRect(renderer, bRectF, radius);
            } else {
              SDL_RenderDrawRect(renderer, &bRect);
            }
          }
        }
        if (data.cancelTextTexture) {
          SDL_RenderCopy(renderer, data.cancelTextTexture, nullptr, &data.cancelTextRect);
        }
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
    data.okText = luaStateWrapper.getTableString(tableName, Keys::OK_TEXT, data.okText);
    data.cancelText = luaStateWrapper.getTableString(tableName, Keys::CANCEL_TEXT, data.cancelText);

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
    SDL_Color fc = ColorUtils::hexToRGB(fontColorHex, Constants::FULL_ALPHA);
    data.titleColor = fc;
    data.subtitleColor = fc;
    data.messageColor = fc;

    data.conditionFunc = luaStateWrapper.getTableString(tableName, Keys::CONDITION, Constants::EMPTY_STRING);
    data.okCallback = luaStateWrapper.getTableString(tableName, Keys::OK_CALLBACK, Constants::EMPTY_STRING);
    data.cancelCallback = luaStateWrapper.getTableString(tableName, Keys::CANCEL_CALLBACK, Constants::EMPTY_STRING);
    std::string typeStr = luaStateWrapper.getTableString(tableName, Keys::TYPE, Constants::EMPTY_STRING);
    data.modalType = ModalTypeResolver::resolve(typeStr);

    createTitleTexture();
    createSubtitleTexture();
    createMessageTexture();
    if (data.modalType == ModalType::NOTIFICATION || data.modalType == ModalType::QUESTION) {
      createOkTextTexture();
      if (data.modalType == ModalType::QUESTION) {
        createCancelTextTexture();
      }
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
      if (s.opacity != 1.0f) {
        if (s.opacity > 1.0f) {
          data.color.a = static_cast<Uint8>(std::min(s.opacity, static_cast<float>(Constants::FULL_ALPHA)));
        } else {
          data.color.a = static_cast<Uint8>(s.opacity * Constants::FULL_ALPHA);
        }
      }
      if (s.fontColor.a != 0) {
        data.titleColor = s.fontColor;
        data.subtitleColor = s.fontColor;
        data.messageColor = s.fontColor;
        data.okTextColor = s.fontColor;
      }
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
      if (s.borderRadius > 0) data.borderRadius = static_cast<int>(s.borderRadius);
      if (s.paddingTop || s.paddingRight || s.paddingBottom || s.paddingLeft) {
        data.paddingTop = s.paddingTop;
        data.paddingRight = s.paddingRight;
        data.paddingBottom = s.paddingBottom;
        data.paddingLeft = s.paddingLeft;
      }

      Style sub = StyleManager::getInstance().getStyle(selector + "-subtitle");
      if (sub.fontColor.a != 0) data.subtitleColor = sub.fontColor;
      if (sub.opacity != 1.0f) {
        if (sub.opacity > 1.0f) {
          data.subtitleColor.a = static_cast<Uint8>(std::min(sub.opacity, static_cast<float>(Constants::FULL_ALPHA)));
        } else {
          data.subtitleColor.a = static_cast<Uint8>(sub.opacity * Constants::FULL_ALPHA);
        }
      }

      Style ttl = StyleManager::getInstance().getStyle(selector + "-title");
      if (ttl.fontColor.a != 0) data.titleColor = ttl.fontColor;
      if (ttl.opacity != 1.0f) {
        if (ttl.opacity > 1.0f) {
          data.titleColor.a = static_cast<Uint8>(std::min(ttl.opacity, static_cast<float>(Constants::FULL_ALPHA)));
        } else {
          data.titleColor.a = static_cast<Uint8>(ttl.opacity * Constants::FULL_ALPHA);
        }
      }

      Style txt = StyleManager::getInstance().getStyle(selector + "-text");
      if (txt.fontColor.a != 0) data.messageColor = txt.fontColor;
      if (txt.opacity != 1.0f) {
        if (txt.opacity > 1.0f) {
          data.messageColor.a = static_cast<Uint8>(std::min(txt.opacity, static_cast<float>(Constants::FULL_ALPHA)));
        } else {
          data.messageColor.a = static_cast<Uint8>(txt.opacity * Constants::FULL_ALPHA);
        }
      }

      Style btn = StyleManager::getInstance().getStyle(selector + "-button");
      if (btn.width > 0) data.okRect.w = btn.width;
      if (btn.height > 0) data.okRect.h = btn.height;
      if (btn.background.a != 0) data.okColor = btn.background;
      if (btn.fontColor.a != 0) data.okTextColor = btn.fontColor;
      if (btn.borderColor.a != 0) data.okBorderColor = btn.borderColor;
      if (btn.borderWidth > 0) data.okBorderWidth = btn.borderWidth;
      if (btn.borderRadius > 0) data.okBorderRadius = static_cast<int>(btn.borderRadius);
      if (btn.opacity != 1.0f) {
        if (btn.opacity > 1.0f) {
          data.okColor.a = static_cast<Uint8>(std::min(btn.opacity, static_cast<float>(Constants::FULL_ALPHA)));
        } else {
          data.okColor.a = static_cast<Uint8>(btn.opacity * Constants::FULL_ALPHA);
        }
      }

      Style cancelBtn = StyleManager::getInstance().getStyle(selector + "-cancel-button");
      if (cancelBtn.width > 0) data.cancelRect.w = cancelBtn.width;
      if (cancelBtn.height > 0) data.cancelRect.h = cancelBtn.height;
      if (cancelBtn.background.a != 0) data.cancelColor = cancelBtn.background;
      if (cancelBtn.fontColor.a != 0) data.cancelTextColor = cancelBtn.fontColor;
      if (cancelBtn.borderColor.a != 0) data.cancelBorderColor = cancelBtn.borderColor;
      if (cancelBtn.borderWidth > 0) data.cancelBorderWidth = cancelBtn.borderWidth;
      if (cancelBtn.borderRadius > 0) data.cancelBorderRadius = static_cast<int>(cancelBtn.borderRadius);
      if (cancelBtn.opacity != 1.0f) {
        if (cancelBtn.opacity > 1.0f) {
          data.cancelColor.a = static_cast<Uint8>(std::min(cancelBtn.opacity, static_cast<float>(Constants::FULL_ALPHA)));
        } else {
          data.cancelColor.a = static_cast<Uint8>(cancelBtn.opacity * Constants::FULL_ALPHA);
        }
      }
    }

    createTitleTexture();
    createSubtitleTexture();
    createMessageTexture();
    if (data.modalType == ModalType::NOTIFICATION) {
      createOkTextTexture();
    }
  }

  void ModalComponent::setEntityPosition(float x, float y) {
    data.rect.x = static_cast<int>(x);
    data.rect.y = static_cast<int>(y);
    createTitleTexture();
    createSubtitleTexture();
    createMessageTexture();
  }

  void ModalComponent::setSize(int w, int h) {
    data.rect.w = w;
    data.rect.h = h;
    createTitleTexture();
    createSubtitleTexture();
    createMessageTexture();
  }

  void ModalComponent::createTitleTexture() {
    if (!renderer || !data.font) return;
    if (data.titleTexture) {
      SDL_DestroyTexture(data.titleTexture);
      data.titleTexture = nullptr;
    }
    if (data.title.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(data.font, data.title.c_str(), data.titleColor);
    if (!surface) return;
    data.titleTexture = SDL_CreateTextureFromSurface(renderer, surface);
    data.titleRect = {data.rect.x + data.paddingLeft, data.rect.y + data.paddingTop, surface->w, surface->h};
    SDL_FreeSurface(surface);
  }

  void ModalComponent::createSubtitleTexture() {
    if (!renderer || !data.font) return;
    if (data.subtitleTexture) {
      SDL_DestroyTexture(data.subtitleTexture);
      data.subtitleTexture = nullptr;
    }
    if (data.subtitle.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(data.font, data.subtitle.c_str(), data.subtitleColor);
    if (!surface) return;
    int y = data.rect.y + data.paddingTop;
    if (data.titleTexture) {
      y = data.titleRect.y + data.titleRect.h + Constants::DEFAULT_TEXT_MARGIN;
    }
    data.subtitleTexture = SDL_CreateTextureFromSurface(renderer, surface);
    data.subtitleRect = {data.rect.x + data.paddingLeft, y, surface->w, surface->h};
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
      } else if (data.modalType == ModalType::QUESTION) {
        positionQuestionButtons();
      }
      return;
    }

    SDL_Surface* surface = TTF_RenderText_Blended(data.font, data.message.c_str(), data.messageColor);
    if (!surface) {
      if (data.modalType == ModalType::NOTIFICATION) {
        positionOkButton();
      } else if (data.modalType == ModalType::QUESTION) {
        positionQuestionButtons();
      }
      return;
    }

    int y = data.rect.y + data.paddingTop;
    if (data.subtitleTexture) {
      y = data.subtitleRect.y + data.subtitleRect.h + Constants::DEFAULT_TEXT_HEIGHT_OFFSET;
    } else if (data.titleTexture) {
      y = data.titleRect.y + data.titleRect.h + Constants::DEFAULT_TEXT_HEIGHT_OFFSET;
    }
    data.messageTexture = SDL_CreateTextureFromSurface(renderer, surface);
    data.messageRect = {data.rect.x + data.paddingLeft, y, surface->w, surface->h};
    SDL_FreeSurface(surface);
    if (data.modalType == ModalType::NOTIFICATION) {
      positionOkButton();
    } else if (data.modalType == ModalType::QUESTION) {
      positionQuestionButtons();
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
    if (data.modalType == ModalType::NOTIFICATION) {
      positionOkButton();
    } else if (data.modalType == ModalType::QUESTION) {
      positionQuestionButtons();
    }
  }

  void ModalComponent::createCancelTextTexture() {
    if (!renderer || !data.font) return;
    if (data.cancelTextTexture) {
      SDL_DestroyTexture(data.cancelTextTexture);
      data.cancelTextTexture = nullptr;
    }
    SDL_Surface* surface = TTF_RenderText_Blended(data.font, data.cancelText.c_str(), data.cancelTextColor);
    if (!surface) return;
    data.cancelTextTexture = SDL_CreateTextureFromSurface(renderer, surface);
    data.cancelTextRect.w = surface->w;
    data.cancelTextRect.h = surface->h;
    SDL_FreeSurface(surface);
    positionQuestionButtons();
  }

  void ModalComponent::positionOkButton() {
    data.okRect.x = data.rect.x + (data.rect.w - data.okRect.w) / Constants::INDEX_TWO;
    data.okRect.y = data.rect.y + data.rect.h - data.okRect.h - data.paddingBottom;
    data.okTextRect.x = data.okRect.x + (data.okRect.w - data.okTextRect.w) / Constants::INDEX_TWO;
    data.okTextRect.y = data.okRect.y + (data.okRect.h - data.okTextRect.h) / Constants::INDEX_TWO;
  }

  void ModalComponent::positionQuestionButtons() {
    int totalWidth = data.okRect.w + data.cancelRect.w + Constants::DEFAULT_TEXT_MARGIN;
    int startX = data.rect.x + (data.rect.w - totalWidth) / Constants::INDEX_TWO;
    data.okRect.x = startX;
    data.cancelRect.x = startX + data.okRect.w + Constants::DEFAULT_TEXT_MARGIN;
    data.okRect.y = data.cancelRect.y = data.rect.y + data.rect.h - data.okRect.h - data.paddingBottom;
    data.okTextRect.x = data.okRect.x + (data.okRect.w - data.okTextRect.w) / Constants::INDEX_TWO;
    data.okTextRect.y = data.okRect.y + (data.okRect.h - data.okTextRect.h) / Constants::INDEX_TWO;
    data.cancelTextRect.x = data.cancelRect.x + (data.cancelRect.w - data.cancelTextRect.w) / Constants::INDEX_TWO;
    data.cancelTextRect.y = data.cancelRect.y + (data.cancelRect.h - data.cancelTextRect.h) / Constants::INDEX_TWO;
  }
}
