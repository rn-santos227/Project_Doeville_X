#ifndef MODAL_COMPONENT_DATA_H
#define MODAL_COMPONENT_DATA_H

#include "ModalType.h"

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

namespace Project::Components {
  struct ModalComponentData {
    ModalType modalType{ModalType::NOTIFICATION};

    SDL_Rect rect{0, 0, 0, 0};
    SDL_Color color{255, 255, 255, 255};
    SDL_Color borderColor{0, 0, 0, 0};
    int borderWidth{0};

    TTF_Font* font = nullptr;
    SDL_Color fontColor{255, 255, 255, 255};
    int fontSize{16};

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

    std::string okText{"OK"};
    SDL_Color okTextColor{0, 0, 0, 255};
    SDL_Texture* okTextTexture = nullptr;
    SDL_Rect okTextRect{0, 0, 0, 0};

    bool okWasPressed{false};
    bool dismissed{false};

    std::string conditionFunc;
    std::string okCallback;
  };
}

#endif
