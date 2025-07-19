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
}
