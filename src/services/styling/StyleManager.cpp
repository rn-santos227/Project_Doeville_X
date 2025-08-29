#include "StyleBinding.h"
#include "StyleManager.h"
#include "StylePropertyResolver.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>

#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::Services {
  using Project::Services::StylePropertyResolver;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  StyleManager& StyleManager::getInstance() {
    static StyleManager instance;
    return instance;
  }

  bool StyleManager::loadFromString(const std::string& css) {
    std::string cleaned = StyleBinding::stripComments(css);
    if (!StyleBinding::hasBalancedBraces(cleaned)) {
      std::cerr << "Unbalanced braces in CSS.\n";
      return false;
    }

    std::regex ruleRegex(Constants::REGEX_CSS_RULE);
    std::smatch match;
    std::string::const_iterator searchStart(cleaned.cbegin());

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
            StyleBinding::setBackground(style, value);
            break;

          case StyleProperty::FOREGROUND:
            StyleBinding::setForeground(style, value);
            break;

          case StyleProperty::HOVER_COLOR:
            StyleBinding::setHoverColor(style, value);
            break;

          case StyleProperty::BORDER_COLOR:
            StyleBinding::setBorderColor(style, value);
            break;

          case StyleProperty::BORDER_WIDTH:
            StyleBinding::setBorderWidth(style, value);
            break;

          case StyleProperty::BORDER_RADIUS:
            StyleBinding::setBorderRadius(style, value);
            break;

          case StyleProperty::FONT_COLOR:
            StyleBinding::setFontColor(style, value);
            break;

          case StyleProperty::FONT_HOVER_COLOR:
            StyleBinding::setFontHoverColor(style, value);
            break;

          case StyleProperty::FONT_SIZE:
            StyleBinding::setFontSize(style, value);
            break;

          case StyleProperty::GRADIENT:
            StyleBinding::setGradient(style, value);
            break;

          case StyleProperty::GRADIENT_START:
            StyleBinding::setGradientStart(style, value);
            break;
          
          case StyleProperty::GRADIENT_END:
            StyleBinding::setGradientEnd(style, value);
            break;

          case StyleProperty::GRADIENT_STYLE: 
            StyleBinding::setGradientStyle(style, value);
            break;

          case StyleProperty::MARGIN: 
            StyleBinding::setMargin(style, value);
            break;
          
          case StyleProperty::MARGIN_TOP:
            StyleBinding::setMarginTop(style, value);
            break;
          
          case StyleProperty::MARGIN_RIGHT:
            StyleBinding::setMarginRight(style, value);
            break;
          
          case StyleProperty::MARGIN_BOTTOM:
            StyleBinding::setMarginLeft(style, value);
            break;
          
          case StyleProperty::MARGIN_LEFT:
            StyleBinding::setMarginLeft(style, value);
            break;

          case StyleProperty::PADDING:
            StyleBinding::setPadding(style, value);
            break;

          case StyleProperty::PADDING_TOP:
            StyleBinding::setPaddingTop(style, value);
            break;
          
          case StyleProperty::PADDING_RIGHT:
            StyleBinding::setPaddingRight(style, value);
            break;
          
          case StyleProperty::PADDING_BOTTOM:
            StyleBinding::setPaddingBottom(style, value);
            break;
          
          case StyleProperty::PADDING_LEFT:
            StyleBinding::setPaddingLeft(style, value);
            break;

          case StyleProperty::WIDTH:
            StyleBinding::setWidth(style, value);
            break;

          case StyleProperty::HEIGHT:
            StyleBinding::setHeight(style, value);
            break;

          case StyleProperty::OPACITY:
            StyleBinding::setOpacity(style, value);
            break;

          default:
            break;
        }
        propStart = propMatch.suffix().first;
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
}
