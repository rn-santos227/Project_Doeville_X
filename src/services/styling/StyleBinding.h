#ifndef STYLE_BINDING_H
#define STYLE_BINDING_H

#include "Style.h"

#include <string>

namespace Project::Services::StyleBinding {
  void setBackground(Style& style, const std::string& value);
  void setForeground(Style& style, const std::string& value);
  void setHoverColor(Style& style, const std::string& value);
  void setBorderColor(Style& style, const std::string& value);
  void setBorderWidth(Style& style, const std::string& value);
  void setBorderRadius(Style& style, const std::string& value);
  void setFontColor(Style& style, const std::string& value);
  void setFontHoverColor(Style& style, const std::string& value);
  void setFontSize(Style& style, const std::string& value);
  void setGradient(Style& style, const std::string& value);
  void setGradientStart(Style& style, const std::string& value);
  void setGradientEnd(Style& style, const std::string& value);
  void setGradientStyle(Style& style, const std::string& value);
  void setMargin(Style& style, const std::string& value);
  void setMarginTop(Style& style, const std::string& value);
  void setMarginRight(Style& style, const std::string& value);
  void setMarginBottom(Style& style, const std::string& value);
  void setMarginLeft(Style& style, const std::string& value);
  void setPadding(Style& style, const std::string& value);
  void setPaddingTop(Style& style, const std::string& value);
  void setPaddingRight(Style& style, const std::string& value);
  void setPaddingBottom(Style& style, const std::string& value);
  void setPaddingLeft(Style& style, const std::string& value);
  void setWidth(Style& style, const std::string& value);
  void setHeight(Style& style, const std::string& value);
  void setOpacity(Style& style, const std::string& value);

  std::string stripComments(const std::string& css);
  bool hasBalancedBraces(const std::string& css);
}

#endif
