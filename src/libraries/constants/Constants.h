#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL.h>
#include <string>

namespace Project::Libraries {
  const std::string PROJECT_NAME = "Project Doeville X";
  const std::string VERSION = "1.0.0";

  // File paths
  constexpr const char* LOGS_DIRECTORY = "resources/logs";
  constexpr const char* LOG_FILE_PATH = "resources/logs/logs.txt";
  constexpr const char* DEFAULT_CURSOR_PATH = "resources/system/cursor_default.png";
  constexpr const char* SCRIPT_FOLDER = "scripts/";
  constexpr const char* ENTITY_SCRIPT_FOLDER = "scripts/entities/";
  constexpr const char* DEFAULT_FONT_PATH = "resources/fonts/system.ttf";
} 

#endif 
