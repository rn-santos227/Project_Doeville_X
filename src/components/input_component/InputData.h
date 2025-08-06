#ifndef INPUT_DATA_H
#define INPUT_DATA_H

#include "InputType.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <array>
#include <string>

#include "libraries/constants/Constants.h"

namespace Project::Components {
  struct InputData {
    SDL_Rect rect{0, 0, 0, 0};
    SDL_Color bgColor = Project::Libraries::Constants::COLOR_BLACK;
    SDL_Color textColor = Project::Libraries::Constants::COLOR_WHITE;
    SDL_Color borderColor = Project::Libraries::Constants::COLOR_BLACK;

    std::array<Uint8, SDL_NUM_SCANCODES> prevKeys{};

    InputType inputType{InputType::TEXT};

    std::string fontPath;
    std::string currentText;
    std::string placeholder;

    float backspaceTimer{0.0f};
    float cursorBlink{0.0f};

    int fontSize{Project::Libraries::Constants::DEBUG_FONT_SIZE};
    int borderWidth{0};
    int borderRadius{0};
    int textOffset{0};
    int textureW{0};
    int textureH{0};

    int paddingTop{0};
    int paddingRight{0};
    int paddingBottom{0};
    int paddingLeft{0};

    std::size_t maxChars{Project::Libraries::Constants::DEFAULT_MAX_CHARS};
    std::size_t caretPos{0};

    bool activeInput{false};
    bool backspaceHeld{false};    
    bool hovered{false};
    bool mousePrev{false};
    bool showCaret{true};
  };
}

#endif
