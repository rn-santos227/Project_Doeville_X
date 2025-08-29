#ifndef CSS_PROPERTY_KEYS_H
#define CSS_PROPERTY_KEYS_H

namespace Project::Libraries::Keys {
  // Box Model
  constexpr const char* CSS_WIDTH = "width";
  constexpr const char* CSS_HEIGHT = "height";
  constexpr const char* CSS_MIN_WIDTH = "min-width";
  constexpr const char* CSS_MIN_HEIGHT = "min-height";
  constexpr const char* CSS_MAX_WIDTH = "max-width";
  constexpr const char* CSS_MAX_HEIGHT = "max-height";

  constexpr const char* CSS_MARGIN = "margin";
  constexpr const char* CSS_MARGIN_TOP = "margin-top";
  constexpr const char* CSS_MARGIN_RIGHT = "margin-right";
  constexpr const char* CSS_MARGIN_BOTTOM = "margin-bottom";
  constexpr const char* CSS_MARGIN_LEFT= "margin-left";

  constexpr const char* CSS_PADDING= "padding";
  constexpr const char* CSS_PADDING_TOP= "padding-top";
  constexpr const char* CSS_PADDING_RIGHT = "padding-right";
  constexpr const char* CSS_PADDING_BOTTOM = "padding-bottom";
  constexpr const char* CSS_PADDING_LEFT = "padding-left";

  // Borders
  constexpr const char* CSS_BORDER_COLOR = "border-color";
  constexpr const char* CSS_BORDER_WIDTH = "border-width";
  constexpr const char* CSS_BORDER_RADIUS = "border-radius";

  // Background
  constexpr const char* CSS_BACKGROUND = "background";
  constexpr const char* CSS_BACKGROUND_COLOR = "background-color";
  constexpr const char* CSS_BACKGROUND_IMAGE = "background-image";
  constexpr const char* CSS_BACKGROUND_REPEAT = "background-repeat";
  constexpr const char* CSS_BACKGROUND_SIZE  = "background-size";
  constexpr const char* CSS_GRADIENT  = "gradient";
  constexpr const char* CSS_GRADIENT_START= "gradient-start";
  constexpr const char* CSS_GRADIENT_END = "gradient-end";
  constexpr const char* CSS_GRADIENT_STYLE = "gradient-style";

  // Text & Font
  constexpr const char* CSS_FONT_FAMILY= "font-family";
  constexpr const char* CSS_FONT_SIZE = "font-size";
  constexpr const char* CSS_FONT_COLOR = "font-color";
  constexpr const char* CSS_FONT_HOVER_COLOR = "font-hover-color";
  constexpr const char* CSS_FONT_WEIGHT = "font-weight";
  constexpr const char* CSS_TEXT_ALIGN = "text-align";

  // Foreground (useful alias)
  constexpr const char* CSS_FOREGROUND = "foreground";

  // Layout
  constexpr const char* CSS_DISPLAY = "display";
  constexpr const char* CSS_POSITION = "position"; 
  constexpr const char* CSS_TOP = "top";
  constexpr const char* CSS_RIGHT = "right";
  constexpr const char* CSS_BOTTOM = "bottom";
  constexpr const char* CSS_LEFT = "left";
  constexpr const char* CSS_Z_INDEX = "z-index";

  // Effects & Transforms
  constexpr const char* CSS_OPACITY = "opacity";
  constexpr const char* CSS_TRANSFORM = "transform";
  constexpr const char* CSS_ROTATE = "rotate";
  constexpr const char* CSS_SCALE = "scale";
  constexpr const char* CSS_TRANSLATE_X = "translate-x";
  constexpr const char* CSS_TRANSLATE_Y = "translate-y";

  // Shadow
  constexpr const char* CSS_BOX_SHADOW = "box-shadow";
  constexpr const char* CSS_TEXT_SHADOW = "text-shadow";

  // Cursor
  constexpr const char* CSS_CURSOR= "cursor";

  // Custom/Engine Specific
  constexpr const char* CSS_WEIGHT = "weight"; 
  constexpr const char* CSS_ANIMATION = "animation";
  constexpr const char* CSS_HOVER_COLOR = "hover-color";

  constexpr const char* POS_STATIC = "static";
  constexpr const char* POS_RELATIVE = "relative"; 
  constexpr const char* POS_ABSOLUTE = "absolute"; 
  constexpr const char* POS_FIXED = "fixed"; 
  constexpr const char* POS_STICKY = "static"; 

  constexpr const char* GRAD_NONE = "none";
  constexpr const char* GRAD_HORIZONTAL = "horizontal";
  constexpr const char* GRAD_VERTICAL = "vertical";
  constexpr const char* GRAD_DIAG_LEFT = "diagonal-left";
  constexpr const char* GRAD_DIAG_RIGHT = "diagonal-right";
  constexpr const char* GRAD_RADIAL = "radial";
  constexpr const char* GRAD_INVERSE = "inverse";

  constexpr const char* DIS_NONE = "none";
  constexpr const char* DIS_BLOCK = "block";
  constexpr const char* DIS_INLINE = "inline";
  constexpr const char* DIS_FLEX = "flex";
  constexpr const char* DIS_GRID = "grid";
  constexpr const char* DIS_INLINE_B = "inline-block";
}

#endif
