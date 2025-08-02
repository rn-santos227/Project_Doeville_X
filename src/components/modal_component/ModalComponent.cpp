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
}
