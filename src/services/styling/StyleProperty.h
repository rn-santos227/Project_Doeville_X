#ifndef STYLE_PROPERTY
#define STYLE_PROPERTY

namespace Project::Services {
  enum class StyleProperty {
    BACKGROUND,
    BORDER_COLOR,
    FOREGROUND,
    GRADIENT_START,
    GRADIENT_END,

    FRONT_SIZE,
    FRONT_WEIGHT,
    MARGIN,
    PADDING,

    RADIUS
  };
}

#endif
