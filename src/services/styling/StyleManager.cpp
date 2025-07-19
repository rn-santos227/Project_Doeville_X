#include "StyleManager.h"

#include <regex>
#include <libcss/libcss.h>
#include <sstream>

#include "utilities/color/ColorUtils.h"

namespace Project::Services {
  StyleManager& StyleManager::getInstance() {
    static StyleManager instance;
    return instance;
  }

  bool StyleManager::loadFromString(const std::string& css) {
    std::regex ruleRegex("(\\.[\\w-]+)\\s*\\{([^}]+)\\}");
    std::smatch match;
    std::string::const_iterator searchStart(css.cbegin());

    while (std::regex_search(searchStart, css.cend(), match, ruleRegex)) {
      std::string selector = match[1].str();
      std::string body = match[2].str();
      Style style;
    }
  }
}
