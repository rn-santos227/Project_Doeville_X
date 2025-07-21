#include "StyleManager.h"
#include "StylePropertyResolver.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>
#include <unordered_map>

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

  inline const std::unordered_map<std::string, GradientType> gradientMap = {
    {std::string(Keys::GRAD_NONE), GradientType::NONE},
    {std::string(Keys::GRAD_HORIZONTAL), GradientType::HORIZONTAL},
    {std::string(Keys::GRAD_VERTICAL), GradientType::VERTICAL},
    {std::string(Keys::GRAD_DIAG_LEFT), GradientType::DIAGONAL_LEFT},
    {std::string(Keys::GRAD_DIAG_RIGHT), GradientType::DIAGONAL_RIGHT},
    {std::string(Keys::GRAD_RADIAL), GradientType::RADIAL},
    {std::string(Keys::GRAD_INVERSE), GradientType::INVERSE}
  };

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
            style.background = ColorUtils::hexToRGB(value);
            break;

          case StyleProperty::FONT_COLOR:
            style.fontColor = ColorUtils::hexToRGB(value);
            break;

          case StyleProperty::FONT_SIZE:
            style.fontSize = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::GRADIENT: {
            std::smatch gradMatch;
            std::regex gradRegex(Constants::REGEX_CSS_GRAD);
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

          case StyleProperty::GRADIENT_STYLE: {
            std::string key = value;
            std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
              return std::tolower(c);
            });

            auto it = gradientMap.find(key);
            style.gradient = (it != gradientMap.end()) ? it->second : GradientType::NONE;
          }

          case StyleProperty::WIDTH:
            style.width = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::HEIGHT:
            style.height = AdvanceParser::parseInt(value);
            break;

          case StyleProperty::OPACITY:
            style.opacity = std::stof(value);
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
