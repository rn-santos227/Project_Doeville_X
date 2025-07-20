#include "StyleManager.h"
#include "StylePropertyResolver.h"

#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/advance_parser/AdvanceParser.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Services {
  using Project::Services::StylePropertyResolver;
  using Project::Utilities::AdvanceParser;
  using Project::Utilities::ColorUtils;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  StyleManager& StyleManager::getInstance() {
    static StyleManager instance;
    return instance;
  }

  bool StyleManager::loadFromString(const std::string& css) {
    std::string cleaned = stripComments(css);
    if (!hasBalancedBraces(cleaned)) {
      std::cerr << "Unbalanced braces in CSS.\n";
      return false;
    }

    std::regex ruleRegex(Constants::REGEX_CSS_RULE);
    std::smatch match;
    std::string::const_iterator searchStart(css.cbegin());

    while (std::regex_search(searchStart, cleaned.cend(), match, ruleRegex)) {
      std::string selector = match[1].str();
      std::string body = match[2].str();
      Style style;

      std::regex propRegex(Constants::REGEX_CSS_PROP);
      std::smatch propMatch;
      std::string::const_iterator propStart(body.cbegin());
      while (std::regex_search(propStart, body.cend(), propMatch, propRegex)) {
        std::string prop = propMatch[1].str();
        std::string value = propMatch[2].str();

        StyleProperty property = StylePropertyResolver::resolve(prop);
        switch (property) {
          case StyleProperty::BACKGROUND:
          case StyleProperty::BACKGROUND_COLOR:
            style.background = ColorUtils::hexToRGB(value);
            break;

          case StyleProperty::FOREGROUND:
            style.foreground = ColorUtils::hexToRGB(value);
            break;

          case StyleProperty::BORDER_COLOR:
            style.borderColor = ColorUtils::hexToRGB(value);
            break;

          case StyleProperty::GRADIENT: {
            std::regex gradRegex(Constants::REGEX_CSS_GRAD);
            std::smatch gradMatch;
            if (std::regex_match(value, gradMatch, gradRegex)) {
              style.gradientStart = ColorUtils::hexToRGB(gradMatch[1].str());
              style.gradientEnd = ColorUtils::hexToRGB(gradMatch[2].str());
              style.hasGradient = true;
            }
            break;
          }

          case StyleProperty::GRADIENT_START:
            style.gradientStart = ColorUtils::hexToRGB(value);
            style.hasGradient = true;
            break;

          case StyleProperty::GRADIENT_END:
            style.gradientEnd = ColorUtils::hexToRGB(value);
            style.hasGradient = true;
            break;

          case StyleProperty::FONT_COLOR:
            style.fontColor = ColorUtils::hexToRGB(value);
            break;

          case StyleProperty::WIDTH:
            style.width = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::HEIGHT:
            style.height = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::MIN_WIDTH:
            style.minWidth = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::MIN_HEIGHT:
            style.minHeight = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::MAX_WIDTH:
            style.maxWidth = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::MAX_HEIGHT:
            style.maxHeight = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::BORDER_WIDTH:
            style.borderWidth = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::BORDER_RADIUS:
            style.borderRadius = static_cast<float>(std::stof(value));
            break;

          case StyleProperty::MARGIN:
            AdvanceParser::parseSpacing(value, style.marginTop, style.marginRight, style.marginBottom, style.marginLeft);
            break;

          case StyleProperty::MARGIN_TOP:
            style.marginTop = AdvanceParser::parseInt(value);
            break;
            
          case StyleProperty::MARGIN_RIGHT:
            style.marginRight = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::MARGIN_BOTTOM:
            style.marginBottom = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::MARGIN_LEFT:
            style.marginLeft = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::PADDING:
            AdvanceParser::parseSpacing(value, style.paddingTop, style.paddingRight, style.paddingBottom, style.paddingLeft);
            break;

          case StyleProperty::PADDING_TOP:
            style.paddingTop = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::PADDING_RIGHT:
            style.paddingRight = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::PADDING_BOTTOM:
            style.paddingBottom = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::PADDING_LEFT:
            style.paddingLeft = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::TOP:
            style.top = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::RIGHT:
            style.right = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::BOTTOM:
            style.bottom = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::LEFT:
            style.left = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::Z_INDEX:
            style.zIndex = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::POSITION: {
            std::string valLower = value;
            std::transform(valLower.begin(), valLower.end(), valLower.begin(), ::tolower);
            if (valLower == "absolute") style.position = PositionType::ABSOLUTE;
            else if (valLower == "relative") style.position = PositionType::RELATIVE;
            else if (valLower == "fixed") style.position = PositionType::FIXED;
            else if (valLower == "sticky") style.position = PositionType::STICKY;
            else style.position = PositionType::STATIC;
            break;
          }

          case StyleProperty::DISPLAY: {
            std::string valLower = value;
            std::transform(valLower.begin(), valLower.end(), valLower.begin(), ::tolower);
            if (valLower == "none") style.display = DisplayType::NONE;
            else if (valLower == "block") style.display = DisplayType::BLOCK;
            else if (valLower == "inline") style.display = DisplayType::INLINE;
            else if (valLower == "flex") style.display = DisplayType::FLEX;
            else if (valLower == "grid") style.display = DisplayType::GRID;
            else if (valLower == "inline-block") style.display = DisplayType::INLINE_BLOCK;
            break;
          }

          case StyleProperty::OPACITY:
            style.opacity = std::stof(value);
            break;
          case StyleProperty::ROTATE:
            style.rotate = std::stof(value);
            break;
          case StyleProperty::SCALE:
            style.scale = std::stof(value);
            break;
          case StyleProperty::TRANSLATE_X:
            style.translateX = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::TRANSLATE_Y:
            style.translateY = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::CURSOR:
            style.cursor = value;
            break;

          default:
            break;
        }
      }
    
      styles[selector] = style;
      searchStart = match.suffix().first;
    }
    return true;
  }

  Style StyleManager::getStyle(const std::string& selector) const {
    auto it = styles.find(selector);
    if (it != styles.end()) {
      return it->second;
    }
    return Style{};
  }

  std::string StyleManager::stripComments(const std::string& css) {
    std::regex commentRegex(Constants::REGEX_CSS_COMMENTS);
    return std::regex_replace(css, commentRegex, Constants::EMPTY_STRING);
  }

  bool StyleManager::hasBalancedBraces(const std::string& css) {
    size_t openCount = std::count(css.begin(), css.end(), '{');
    size_t closeCount = std::count(css.begin(), css.end(), '}');
    return openCount == closeCount;
  }
}
