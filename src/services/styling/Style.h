#ifndef STYLE_H
#define STYLE_H

#include "StyleProperty.h"

#include <SDL.h>
#include <string>
namespace Project::Services {
  struct Style {
    SDL_Color background{0, 0, 0, 0};
    SDL_Color foreground{0, 0, 0, 0};
    SDL_Color borderColor{0, 0, 0, 0};
    SDL_Color gradientStart{0, 0, 0, 0};
    SDL_Color gradientEnd{0, 0, 0, 0};
    SDL_Color fontColor{0, 0, 0, 0};
    SDL_Color hoverColor{0, 0, 0, 0};
    SDL_Color fontHoverColor{0, 0, 0, 0};

    int width{0};
    int height{0};
    int minWidth{0};
    int minHeight{0};
    int maxWidth{0};
    int maxHeight{0};

    int borderWidth{0};
    float borderRadius{0.0f};

    int marginTop{0};
    int marginRight{0};
    int marginBottom{0};
    int marginLeft{0};

    int paddingTop{0};
    int paddingRight{0};
    int paddingBottom{0};
    int paddingLeft{0};

    int fontSize{0};
    int fontWeight{0};
    std::string fontFamily {"system"};

    int top{0};
    int right{0};
    int bottom{0};
    int left{0};
    int zIndex{0};

    DisplayType display{DisplayType::BLOCK};
    GradientType gradient{GradientType::HORIZONTAL};
    PositionType position{PositionType::STATIC};
    
    int shadowOffsetX{0};
    int shadowOffsetY{0};
    int shadowBlur{0};
    SDL_Color shadowColor{0, 0, 0, 0};

    float rotate{0.0f};
    float scale{1.0f};
    int translateX{0};
    int translateY{0};

    std::string cursor{"system"};
    float opacity{1.0f};

    bool hasGradient{false};
  };
}

#endif
