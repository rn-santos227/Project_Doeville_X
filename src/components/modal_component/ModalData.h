#ifndef MODAL_DATA_H
#define MODAL_DATA_H

#include "ModalType.h"

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

#include "libraries/constants/ColorConstants.h"
#include "libraries/constants/NameConstants.h"
#include "libraries/constants/NumericConstants.h"

namespace Project::Components {
  namespace Constants = Project::Libraries::Constants;
  struct ModalData {
    ModalType modalType{ModalType::NOTIFICATION};

    SDL_Rect rect{0, 0, 0, 0};

    SDL_Color color = Constants::COLOR_WHITE;
    SDL_Color borderColor = Constants::COLOR_BLACK;

    int borderWidth{0};
    int borderRadius{0};

    int paddingTop{0};
    int paddingRight{0};
    int paddingBottom{0};
    int paddingLeft{0};

    TTF_Font* font = nullptr;
    SDL_Color titleColor = Constants::COLOR_WHITE;
    SDL_Color subtitleColor = Constants::COLOR_WHITE;
    SDL_Color messageColor = Constants::COLOR_WHITE;
    int fontSize = Constants::DEFAULT_FONT_SIZE;

    std::string title;
    SDL_Texture* titleTexture = nullptr;
    SDL_Rect titleRect{0, 0, 0, 0};

    std::string subtitle;
    SDL_Texture* subtitleTexture = nullptr;
    SDL_Rect subtitleRect{0, 0, 0, 0};

    std::string message;
    SDL_Texture* messageTexture = nullptr;
    SDL_Rect messageRect{0, 0, 0, 0};

    SDL_Rect okRect{0, 0, 80, 30};
    SDL_Color okColor{200, 200, 200, 255};
    SDL_Color okBorderColor{0, 0, 0, 0};
    int okBorderWidth{0};
    int okBorderRadius{0};

    std::string okText = Constants::BTN_OKAY;
    SDL_Color okTextColor{0, 0, 0, 255};
    SDL_Texture* okTextTexture = nullptr;
    SDL_Rect okTextRect{0, 0, 0, 0};

    SDL_Rect cancelRect{0, 0, 80, 30};
    SDL_Color cancelColor{200, 200, 200, 255};
    SDL_Color cancelBorderColor{0, 0, 0, 0};
    int cancelBorderWidth{0};
    int cancelBorderRadius{0};

    std::string cancelText = Constants::BTN_CANCEL;
    SDL_Color cancelTextColor{0, 0, 0, 255};
    SDL_Texture* cancelTextTexture = nullptr;
    SDL_Rect cancelTextRect{0, 0, 0, 0};

    bool okWasPressed{false};
    bool cancelWasPressed{false};
    bool dismissed{false};

    std::string conditionFunc;
    std::string okCallback;
    std::string cancelCallback;
  };
}

#endif
