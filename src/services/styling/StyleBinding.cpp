#include "StyleBinding.h"

#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/advance_parser/AdvanceParser.h"
#include "utilities/color/ColorUtils.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <unordered_map>

namespace Project::Services::StyleBinding {
  using Project::Utilities::AdvanceParser;
  using Project::Utilities::ColorUtils;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  namespace {
    const std::unordered_map<std::string, GradientType> gradientMap = {
      {std::string(Keys::GRAD_NONE), GradientType::NONE},
      {std::string(Keys::GRAD_HORIZONTAL), GradientType::HORIZONTAL},
      {std::string(Keys::GRAD_VERTICAL), GradientType::VERTICAL},
      {std::string(Keys::GRAD_DIAG_LEFT), GradientType::DIAGONAL_LEFT},
      {std::string(Keys::GRAD_DIAG_RIGHT), GradientType::DIAGONAL_RIGHT},
      {std::string(Keys::GRAD_RADIAL), GradientType::RADIAL},
      {std::string(Keys::GRAD_INVERSE), GradientType::INVERSE}
    };
  }

  void setBackground(Style& style, const std::string& value) {
    style.background = ColorUtils::hexToRGB(value);
  }

  void setForeground(Style& style, const std::string& value) {
    style.foreground = ColorUtils::hexToRGB(value);
  }

  void setHoverColor(Style& style, const std::string& value) {
    style.hoverColor = ColorUtils::hexToRGB(value);
  }

  void setBorderColor(Style& style, const std::string& value) {
    style.borderColor = ColorUtils::hexToRGB(value);
  }

  void setBorderWidth(Style& style, const std::string& value) {
    style.borderWidth = AdvanceParser::parseInt(value);
  }

  void setBorderRadius(Style& style, const std::string& value) {
    style.borderRadius = AdvanceParser::parseFloat(value);
  }

  void setFontColor(Style& style, const std::string& value) {
    style.fontColor = ColorUtils::hexToRGB(value);
  }

  void setFontHoverColor(Style& style, const std::string& value) {
    style.fontHoverColor = ColorUtils::hexToRGB(value);
  }

  void setFontSize(Style& style, const std::string& value) {
    style.fontSize = AdvanceParser::parseInt(value);
  }

  void setGradient(Style& style, const std::string& value) {
    std::smatch gradMatch;
    std::regex gradRegex(Constants::REGEX_CSS_GRAD);
    if (std::regex_match(value, gradMatch, gradRegex)) {
      style.gradientStart = ColorUtils::hexToRGB(gradMatch[1].str());
      style.gradientEnd = ColorUtils::hexToRGB(gradMatch[2].str());
      style.hasGradient = true;
    }
  }

  void setGradientStart(Style& style, const std::string& value) {
    style.gradientStart = ColorUtils::hexToRGB(value);
    style.hasGradient = true;
  }

  void setGradientEnd(Style& style, const std::string& value) {
    style.gradientEnd = ColorUtils::hexToRGB(value);
    style.hasGradient = true;
  }

  void setGradientStyle(Style& style, const std::string& value) {
    std::string key = value;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) {
      return std::tolower(c);
    });
    auto it = gradientMap.find(key);
    style.gradient = (it != gradientMap.end()) ? it->second : GradientType::NONE;
  }

  void setMargin(Style& style, const std::string& value) {
    AdvanceParser::parseSpacing(value, style.marginTop, style.marginRight,
                                style.marginBottom, style.marginLeft);
  }

  void setMarginTop(Style& style, const std::string& value) {
    style.marginTop = AdvanceParser::parseInt(value);
  }

  void setMarginRight(Style& style, const std::string& value) {
    style.marginRight = AdvanceParser::parseInt(value);
  }

  void setMarginBottom(Style& style, const std::string& value) {
    style.marginBottom = AdvanceParser::parseInt(value);
  }

  void setMarginLeft(Style& style, const std::string& value) {
    style.marginLeft = AdvanceParser::parseInt(value);
  }

  void setPadding(Style& style, const std::string& value) {
    AdvanceParser::parseSpacing(value, style.paddingTop, style.paddingRight,
                                style.paddingBottom, style.paddingLeft);
  }

  void setPaddingTop(Style& style, const std::string& value) {
    style.paddingTop = AdvanceParser::parseInt(value);
  }

  void setPaddingRight(Style& style, const std::string& value) {
    style.paddingRight = AdvanceParser::parseInt(value);
  }

  void setPaddingBottom(Style& style, const std::string& value) {
    style.paddingBottom = AdvanceParser::parseInt(value);
  }

  void setPaddingLeft(Style& style, const std::string& value) {
    style.paddingLeft = AdvanceParser::parseInt(value);
  }

  void setWidth(Style& style, const std::string& value) {
    style.width = AdvanceParser::parseInt(value);
  }

  void setHeight(Style& style, const std::string& value) {
    style.height = AdvanceParser::parseInt(value);
  }

  void setOpacity(Style& style, const std::string& value) {
    style.opacity = std::stof(value);
  }

  std::string stripComments(const std::string& css) {
    std::regex commentRegex(Constants::REGEX_CSS_COMMENTS);
    return std::regex_replace(css, commentRegex, Constants::EMPTY_STRING);
  }

  bool hasBalancedBraces(const std::string& css) {
    size_t openCount = std::count(css.begin(), css.end(), '{');
    size_t closeCount = std::count(css.begin(), css.end(), '}');
    return openCount == closeCount;
  }
}
