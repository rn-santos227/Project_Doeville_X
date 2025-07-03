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

  // Screen defaults
  constexpr int DEFAULT_SCREEN_WIDTH = 800;
  constexpr int DEFAULT_SCREEN_HEIGHT = 600;
  constexpr const char* DEFAULT_INITIAL_STATE = "MainMenu";

    // Numeric values
    constexpr int DEFAULT_CURSOR_SIZE = 32;
    constexpr int DEFAULT_COMPONENT_SIZE = 32;
    constexpr int DEBUG_TEXT_MARGIN = 10;
    constexpr int DEFAULT_FONT_SIZE = 24;
    constexpr int DEFAULT_BASE_FONT_SIZE = 16;

    constexpr int MILLISECONDS_PER_SECOND = 1000;
    constexpr int LOG_FLUSH_WAIT_MS = 10;

    constexpr double TARGET_FPS = 60.0;
} 

#endif 
