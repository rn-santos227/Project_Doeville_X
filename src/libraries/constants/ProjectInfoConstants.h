#ifndef PROJECT_INFO_CONSTANTS_H
#define PROJECT_INFO_CONSTANTS_H

#include <string>

namespace Project::Libraries::Constants {
  const std::string PROJECT_NAME = "Project Doeville X";
  const std::string VERSION = "0.1.0";

  constexpr int DEFAULT_SCREEN_WIDTH = 800;
  constexpr int DEFAULT_SCREEN_HEIGHT = 600;
  
  constexpr int DEFAULT_SCREEN_CENTER_X = DEFAULT_SCREEN_WIDTH / 2;
  constexpr int DEFAULT_SCREEN_CENTER_Y = DEFAULT_SCREEN_HEIGHT / 2;
}

#endif
