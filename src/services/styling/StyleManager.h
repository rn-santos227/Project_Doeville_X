#ifndef STYLE_MANAGER_H
#define STYLE_MANAGER_H

#include "Style.h"

#include <string>
#include <unordered_map>

namespace Project::Services {
  class StyleManager {


  private:
    StyleManager() = default;
    std::unordered_map<std::string, Style> styles;
  };
}

#endif
