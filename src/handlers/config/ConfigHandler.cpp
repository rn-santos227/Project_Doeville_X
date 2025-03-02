#include "ConfigHandler.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace Project::Handlers {
  ConfigHandler& ConfigHandler::getInstance() {
    static ConfigHandler instance;
    return instance;
  }
}