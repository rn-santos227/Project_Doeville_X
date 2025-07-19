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

  std::regex ruleRegex("(\\.[\\w-]+)\\s*\\{([^}]+)\\}");
  std::smatch match;
  std::string::const_iterator searchStart(css.cbegin());
}
