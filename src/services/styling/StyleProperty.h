#ifndef STYLE_PROPERTY_H
#define STYLE_PROPERTY_H

namespace Project::Services {
  enum class PositionType {
    STATIC,
    RELATIVE,
    ABSOLUTE,
    FIXED,
    STICKY
  };

  enum class GradientType {
    NONE,
    HORIZONTAL,
    VERTICAL,
    DIAGONAL_LEFT,
    DIAGONAL_RIGHT,
    RADIAL,
    INVERSE
  };

  enum class DisplayType {
    NONE,
    BLOCK,
    INLINE,
    FLEX,
    GRID,
    INLINE_BLOCK
  };
  
  enum class StyleProperty {
    // Background & Foreground
    BACKGROUND,
    BACKGROUND_COLOR,
    FOREGROUND,
    OPACITY,

    // Borders
    BORDER_COLOR,
    BORDER_WIDTH,
    BORDER_RADIUS,

    // Gradients
    GRADIENT,
    GRADIENT_START,
    GRADIENT_END,
    GRADIENT_STYLE,

    // Font/Text
    FONT_COLOR,
    FONT_HOVER_COLOR,
    FONT_SIZE,
    FONT_WEIGHT,
    FONT_FAMILY,
    TEXT_ALIGN,

    // Layout
    WIDTH,
    HEIGHT,
    MIN_WIDTH,
    MIN_HEIGHT,
    MAX_WIDTH,
    MAX_HEIGHT,

    POSITION,
    TOP,
    RIGHT,
    BOTTOM,
    LEFT,
    Z_INDEX,
    DISPLAY,

    // Margin & Padding
    MARGIN,
    MARGIN_TOP,
    MARGIN_RIGHT,
    MARGIN_BOTTOM,
    MARGIN_LEFT,

    PADDING,
    PADDING_TOP,
    PADDING_RIGHT,
    PADDING_BOTTOM,
    PADDING_LEFT,

    // Shadow
    BOX_SHADOW,
    TEXT_SHADOW,

    // Transform
    TRANSFORM,
    ROTATE,
    SCALE,
    TRANSLATE_X,
    TRANSLATE_Y,

    // Cursor & Effects
    CURSOR,
    HOVER_COLOR,

    // Fallback
    UNKNOWN,
  };
}

#endif
