#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL.h>

#include <cstdint>
#include <string>

namespace Project::Libraries::Constants {
  const std::string PROJECT_NAME = "Project Doeville X";
  const std::string VERSION = "0.1.0";

  //Default values for game engine configuration
  constexpr const char* DEFAULT_CURSOR_PATH = "resources/system/cursor_default.png";
  constexpr const char* DEFAULT_ENTITY_SCRIPT_FOLDER = "scripts/entities/";
  constexpr const char* DEFAULT_FONT = "system";
  constexpr const char* DEFAULT_FONT_PATH = "resources/fonts/system.ttf";
  constexpr const char* DEFAULT_INITIAL_STATE = "MainMenu";
  constexpr const char* DEFAULT_LOGS_DIRECTORY = "resources/logs";
  constexpr const char* DEFAULT_LOG_FILE_PATH = "resources/logs/logs.txt";
  constexpr const char* DEFAULT_LUA_LOG_FILE_PATH = "resources/logs/lua_logs.txt";
  constexpr const char* DEFAULT_SCRIPT_PATH = "scripts/";
  constexpr const char* DEFAULT_STATE_SCRIPT_FOLDER = "scripts/states/";
  
  constexpr const char* DEFAULT_ENTITY_NAME = "Default Entity";
  constexpr const char* DEFAULT_LAYER_NAME = "Default Layer";
  constexpr const char* DEFAULT_STATE = "Default State";
  constexpr const char* DEFAULT_TEXT = "Default Text";

  constexpr const char* EMPTY_STRING = "";

  // Indexes used for Arrangements
  constexpr const uint8_t INDEX_ZERO = 0;
  constexpr const uint8_t INDEX_ONE = 1;
  constexpr const uint8_t INDEX_TWO = 2;
  constexpr const uint8_t INDEX_THREE = 3;
  constexpr const uint8_t INDEX_FOUR = 4;
  constexpr const uint8_t INDEX_FIVE = 5;
  constexpr const uint8_t INDEX_SIX = 6;
  constexpr const uint8_t INDEX_SEVEN = 7;
  constexpr const uint8_t INDEX_EIGHT = 8;
  constexpr const uint8_t INDEX_NINE = 9;

  // Screen defaults
  constexpr int DEFAULT_SCREEN_WIDTH = 800;
  constexpr int DEFAULT_SCREEN_HEIGHT = 600;

  // Numeric values
  constexpr int DEBUG_COLUMN_SPACING = 20;
  constexpr int DEBUG_FONT_SIZE = 14;
  constexpr int DEBUG_LINE_SPACING = 5;
  constexpr int DEBUG_PREFIX_WIDTH = 8;
  constexpr int DEBUG_TEXT_MARGIN = 10;
  constexpr int DEBUG_TEXT_HEIGHT_OFFSET = 5;
  constexpr int DEBUG_VALUE_WIDTH = 5;
  constexpr int DEBUG_VALUE_COL_OFFSET_FROM_RIGHT = 10;
  constexpr int DEBUG_PREFIX_COL_OFFSET_FROM_RIGHT = 90;
  constexpr int DEFAULT_CAMERA_CULL_OFFSET = 100;
  constexpr int DEFAULT_CURSOR_SIZE = 32;
  constexpr int DEFAULT_COMPONENT_SIZE = 32;
  constexpr int DEFAULT_FONT_SIZE = 24;
  constexpr int DEFAULT_BASE_FONT_SIZE = 16;
  constexpr int DEFAULT_GRID_SPACING = 32;
  constexpr int DEFAULT_STATE_CACHE_LIMIT = 5;
  constexpr int DEFAULT_DOUBLE = 2;

  // Angle constants in degrees
  constexpr float ANGLE_0_DEG = 0.0f;
  constexpr float ANGLE_30_DEG = 30.0f;
  constexpr float ANGLE_45_DEG = 45.0f;
  constexpr float ANGLE_60_DEG = 60.0f;
  constexpr float ANGLE_90_DEG = 90.0f;
  constexpr float ANGLE_120_DEG = 120.0f;
  constexpr float ANGLE_135_DEG = 135.0f;
  constexpr float ANGLE_180_DEG = 180.0f;
  constexpr float ANGLE_225_DEG = 225.0f;
  constexpr float ANGLE_270_DEG = 270.0f;
  constexpr float ANGLE_315_DEG = 315.0f;
  constexpr float ANGLE_360_DEG = 360.0f;

  // Angle constants in radians (for use with std::sin, std::cos, etc.)
  constexpr float ANGLE_PI_RAD = 3.14159265358979f;
  constexpr float ANGLE_HALF_PI_RAD = ANGLE_PI_RAD / 2.0f;
  constexpr float ANGLE_TWO_PI_RAD = ANGLE_PI_RAD * 2.0f;
  constexpr float DEG_TO_RAD = ANGLE_PI_RAD / 180.0f;
  constexpr float RAD_TO_DEG = 180.0f / ANGLE_PI_RAD;

  constexpr int TIME_YEAR_OFFSET = 1900;
  constexpr int TIME_MONTH_OFFSET = 1;
  constexpr int MILLISECONDS_PER_SECOND = 1000;
  constexpr int LOG_FLUSH_WAIT_MS = 10;
  constexpr double TARGET_FPS = 60.0;
  constexpr double DEFAULT_MAX_FPS = 120.0;
  
  constexpr Uint8 FULL_ALPHA = 255;  
  constexpr size_t BYTES_PER_MEGABYTE = 1024 * 1024;

  // Physics constants
  constexpr float GRAVITY = 9.81f;
  constexpr float TERMINAL_VELOCITY = 500.0f;
  constexpr float DEFAULT_MOTION_SPEED = 100.0f;
  constexpr float DEFAULT_ROTATION_SPEED = 90.0f;
  constexpr float DEFAULT_ACCELERATION = 10.0f;
  constexpr float DEFAULT_FRICTION = 0.1f;
  constexpr float DEFAULT_BOUNCE_FACTOR = 0.5f;
  constexpr float DEFAULT_DAMPING = 0.1f;
  constexpr float DEFAULT_COLLISION_THRESHOLD = 0.1f;
  constexpr float DEFAULT_PUSH_FORCE = 1.0f;
  constexpr float DEFAULT_MASS = 1.0f;
  constexpr float DEFAULT_DENSITY = 0.1f;
  constexpr float DEFAULT_GRAVITY_SCALE = 1.0f;

  // Math constants
  constexpr float DEFAULT_DENOMINATOR = 2.0f;
  constexpr float DEFAULT_WHOLE = 1.0f;
  constexpr float DEFAULT_HALF = 0.5f;
  constexpr SDL_FPoint DEFAULT_GRAVITY_DIRECTION{0.0f, 1.0f};

  // Debug strings
  constexpr const char* DEBUG_FPS_PREFIX = "FPS: ";
  constexpr const char* DEBUG_FRAME_PREFIX = "FRAME: ";
  constexpr const char* DEBUG_FRAME_SUFFIX = " MS";
  constexpr const char* DEBUG_MEM_PREFIX = "MEM: ";
  constexpr const char* DEBUG_MEM_SUFFIX = " MB";
  constexpr const char* DEBUG_PROC_PREFIX = "PROC: ";
  constexpr const char* DEBUG_MOUSE_PREFIX = "Mouse: (";
  constexpr const char* DEBUG_MOUSE_SEPARATOR = ", ";
  constexpr const char* DEBUG_MOUSE_SUFFIX = ")";

  // Colors
  constexpr SDL_Color DEFAULT_DEBUG_TEXT_COLOR{144, 238, 144, 255};
  constexpr SDL_Color DEFAULT_BOUNDING_BOX_COLOR{0, 0, 0, 255};
  constexpr SDL_Color DEFAULT_SHAPE_COLOR{255, 255, 255, 255};
  constexpr SDL_Color DEFAULT_BACKGROUND_COLOR{0, 0, 0, 255};
  constexpr SDL_Color DEFAULT_X_AXIS_COLOR{255, 0, 0, 255};
  constexpr SDL_Color DEFAULT_Y_AXIS_COLOR{0, 0, 255, 255};
  constexpr SDL_Color DEFAULT_Z_AXIS_COLOR{0, 255, 0, 255};
  constexpr SDL_Color DEFAULT_GRID_COLOR{0, 100, 0, 255};

  // Color Hex
  constexpr const char* DEFAULT_DEBUG_TEXT_COLOR_HEX = "#90EE90";
  constexpr const char* DEFAULT_BOUNDING_BOX_COLOR_HEX = "#000000";
  constexpr const char* DEFAULT_SHAPE_COLOR_HEX = "#FFFFFF";

  // Named Primary Colors
  constexpr SDL_Color COLOR_RED{255, 0, 0, 255};
  constexpr SDL_Color COLOR_GREEN{0, 255, 0, 255};
  constexpr SDL_Color COLOR_BLUE{0, 0, 255, 255};

  // Named Secondary Colors
  constexpr SDL_Color COLOR_YELLOW{255, 255, 0, 255};
  constexpr SDL_Color COLOR_CYAN{0, 255, 255, 255};
  constexpr SDL_Color COLOR_MAGENTA{255, 0, 255, 255};

  // Named Grayscale Colors
  constexpr SDL_Color COLOR_BLACK{0, 0, 0, 255};
  constexpr SDL_Color COLOR_WHITE{255, 255, 255, 255};
  constexpr SDL_Color COLOR_GRAY{128, 128, 128, 255};

  // Hex equivalents (optional - useful for tools or debugging)
  constexpr const char* COLOR_HEX_RED = "#FF0000";
  constexpr const char* COLOR_HEX_GREEN = "#00FF00";
  constexpr const char* COLOR_HEX_BLUE = "#0000FF";
  constexpr const char* COLOR_HEX_YELLOW = "#FFFF00";
  constexpr const char* COLOR_HEX_CYAN = "#00FFFF";
  constexpr const char* COLOR_HEX_MAGENTA = "#FF00FF";
  constexpr const char* COLOR_HEX_BLACK = "#000000";
  constexpr const char* COLOR_HEX_WHITE = "#FFFFFF";
  constexpr const char* COLOR_HEX_GRAY = "#808080";

  // Lua script suffixes
  constexpr const char* LUA_STATE_SUFFIX = ".state.lua";
  constexpr const char* LUA_LAYER_SUFFIX = ".layer.lua";
  constexpr const char* LUA_ENTITY_SUFFIX = ".entity.lua";
  constexpr const char* LUA_MAP_SUFFIX = ".map.lua";
  constexpr const char* LUA_ITEM_SUFFIX = ".item.lua";
  constexpr const char* LUA_ANIMATION_SUFFIX = ".animation.lua";

  // Default key bindings
  constexpr SDL_Scancode KEY_FUNC_HELP = SDL_SCANCODE_F1;
  constexpr SDL_Scancode KEY_FUNC_DEBUG = SDL_SCANCODE_F2;
  constexpr SDL_Scancode KEY_FUNC_FREEZE = SDL_SCANCODE_F3;
  constexpr SDL_Scancode KEY_FUNC_RESTART = SDL_SCANCODE_F4;
  constexpr SDL_Scancode KEY_FUNC_EXIT = SDL_SCANCODE_F5;

  // Generic programming script suffixes
  constexpr const char* SCRIPT_LUA_SUFFIX = ".lua";
  constexpr const char* SCRIPT_PYTHON_SUFFIX = ".py";
  constexpr const char* SCRIPT_JAVASCRIPT_SUFFIX = ".js";
  constexpr const char* SCRIPT_TYPESCRIPT_SUFFIX = ".ts";
  constexpr const char* SCRIPT_RUBY_SUFFIX = ".rb";
  constexpr const char* SCRIPT_BASH_SUFFIX = ".sh";
  constexpr const char* SCRIPT_JSON_SUFFIX = ".json";
  constexpr const char* SCRIPT_XML_SUFFIX = ".xml";
  constexpr const char* SCRIPT_YAML_SUFFIX = ".yaml";
  constexpr const char* SCRIPT_TOML_SUFFIX = ".toml";
  constexpr const char* SCRIPT_SQL_SUFFIX = ".sql";
  constexpr const char* SCRIPT_HTML_SUFFIX = ".html";
  constexpr const char* SCRIPT_CSS_SUFFIX = ".css";
  constexpr const char* SCRIPT_TEXT_SUFFIX = ".txt";
  constexpr const char* SCRIPT_INI_SUFFIX = ".ini";

  // Optional: Fallback
  constexpr const char* SCRIPT_GENERIC_SUFFIX = ".script";
} 

#endif 
