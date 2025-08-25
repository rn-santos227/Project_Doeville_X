#include "StylePropertyResolver.h"

#include <array>
#include <string_view>

#include "libraries/keys/CSSPropertyKeys.h"
#include "libraries/constants/NumericConstants.h"
#include "utilities/string/StringUtils.h"

namespace Project::Services {
  namespace Keys = Project::Libraries::Keys;
  namespace Constants = Project::Libraries::Constants;

  StyleProperty StylePropertyResolver::resolve(std::string_view property) {
   static constexpr std::array<std::pair<std::string_view, StyleProperty>, Constants::STYLING_NAME_ALIAS_COUNT> map{{
      {Keys::CSS_BACKGROUND, StyleProperty::BACKGROUND},
      {Keys::CSS_BACKGROUND_COLOR, StyleProperty::BACKGROUND_COLOR},
      {Keys::CSS_FOREGROUND, StyleProperty::FOREGROUND},
      {Keys::CSS_OPACITY, StyleProperty::OPACITY},
      {Keys::CSS_BORDER_COLOR, StyleProperty::BORDER_COLOR},
      {Keys::CSS_BORDER_WIDTH, StyleProperty::BORDER_WIDTH},
      {Keys::CSS_BORDER_RADIUS, StyleProperty::BORDER_RADIUS},
      {Keys::CSS_GRADIENT, StyleProperty::GRADIENT},
      {Keys::CSS_GRADIENT_START, StyleProperty::GRADIENT_START},
      {Keys::CSS_GRADIENT_END,StyleProperty::GRADIENT_END},
      {Keys::CSS_GRADIENT_STYLE, StyleProperty::GRADIENT_STYLE},
      {Keys::CSS_FONT_COLOR, StyleProperty::FONT_COLOR},
      {Keys::CSS_FONT_HOVER_COLOR, StyleProperty::FONT_HOVER_COLOR},
      {Keys::CSS_FONT_SIZE, StyleProperty::FONT_SIZE},
      {Keys::CSS_FONT_WEIGHT, StyleProperty::FONT_WEIGHT},
      {Keys::CSS_FONT_FAMILY, StyleProperty::FONT_FAMILY},
      {Keys::CSS_TEXT_ALIGN, StyleProperty::TEXT_ALIGN},
      {Keys::CSS_WIDTH, StyleProperty::WIDTH},
      {Keys::CSS_HEIGHT, StyleProperty::HEIGHT},
      {Keys::CSS_MIN_WIDTH, StyleProperty::MIN_WIDTH},
      {Keys::CSS_MIN_HEIGHT, StyleProperty::MIN_HEIGHT},
      {Keys::CSS_MAX_WIDTH, StyleProperty::MAX_WIDTH},
      {Keys::CSS_MAX_HEIGHT, StyleProperty::MAX_HEIGHT},
      {Keys::CSS_POSITION,   StyleProperty::POSITION},
      {Keys::CSS_TOP, StyleProperty::TOP},
      {Keys::CSS_RIGHT, StyleProperty::RIGHT},
      {Keys::CSS_BOTTOM, StyleProperty::BOTTOM},
      {Keys::CSS_LEFT, StyleProperty::LEFT},
      {Keys::CSS_Z_INDEX,StyleProperty::Z_INDEX},
      {Keys::CSS_DISPLAY,StyleProperty::DISPLAY},
      {Keys::CSS_MARGIN, StyleProperty::MARGIN},
      {Keys::CSS_MARGIN_TOP, StyleProperty::MARGIN_TOP},
      {Keys::CSS_MARGIN_RIGHT,StyleProperty::MARGIN_RIGHT},
      {Keys::CSS_MARGIN_BOTTOM, StyleProperty::MARGIN_BOTTOM},
      {Keys::CSS_MARGIN_LEFT, StyleProperty::MARGIN_LEFT},
      {Keys::CSS_PADDING, StyleProperty::PADDING},
      {Keys::CSS_PADDING_TOP, StyleProperty::PADDING_TOP},
      {Keys::CSS_PADDING_RIGHT, StyleProperty::PADDING_RIGHT},
      {Keys::CSS_PADDING_BOTTOM, StyleProperty::PADDING_BOTTOM},
      {Keys::CSS_PADDING_LEFT,StyleProperty::PADDING_LEFT},
      {Keys::CSS_BOX_SHADOW, StyleProperty::BOX_SHADOW},
      {Keys::CSS_TEXT_SHADOW, StyleProperty::TEXT_SHADOW},
      {Keys::CSS_TRANSFORM,  StyleProperty::TRANSFORM},
      {Keys::CSS_ROTATE, StyleProperty::ROTATE},
      {Keys::CSS_SCALE, StyleProperty::SCALE},
      {Keys::CSS_TRANSLATE_X, StyleProperty::TRANSLATE_X},
      {Keys::CSS_TRANSLATE_Y, StyleProperty::TRANSLATE_Y},
      {Keys::CSS_CURSOR, StyleProperty::CURSOR},
      {Keys::CSS_HOVER_COLOR, StyleProperty::HOVER_COLOR}
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, property)) return value;
    }
    return StyleProperty::UNKNOWN;
  }
}
