#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL.h>
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
  constexpr const char* DEFAULT_SCRIPT_PATH = "scripts/";
  constexpr const char* DEFAULT_STATE_SCRIPT_FOLDER = "scripts/states/";
  constexpr const char* DEFAULT_TEXT = "Default Text";

  // Screen defaults
  constexpr int DEFAULT_SCREEN_WIDTH = 800;
  constexpr int DEFAULT_SCREEN_HEIGHT = 600;

  // Numeric values
  constexpr int DEBUG_FONT_SIZE = 14;
  constexpr int DEBUG_LINE_SPACING = 5;
  constexpr int DEBUG_TEXT_MARGIN = 10;
  constexpr int DEBUG_TEXT_HEIGHT_OFFSET = 5;
  constexpr int DEBUG_VALUE_WIDTH = 5;

  constexpr int DEFAULT_CURSOR_SIZE = 32;
  constexpr int DEFAULT_COMPONENT_SIZE = 32;
  constexpr int DEFAULT_FONT_SIZE = 24;
  constexpr int DEFAULT_BASE_FONT_SIZE = 16;
  constexpr int DEFAULT_GRID_SPACING = 32;
  constexpr int DEFAULT_STATE_CACHE_LIMIT = 5;

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
  constexpr float TERMINAL_VELOCITY = 50.0f;
  constexpr float DEFAULT_MOTION_SPEED = 100.0f;
  constexpr float DEFAULT_ROTATION_SPEED = 90.0f;
  constexpr float DEFAULT_ACCELERATION = 10.0f;
  constexpr float DEFAULT_FRICTION = 0.1f;
  constexpr float DEFAULT_BOUNCE_FACTOR = 0.5f;
  constexpr float DEFAULT_DAMPING = 0.1f;
  constexpr float DEFAULT_COLLISION_THRESHOLD = 0.1f;

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

  // Lua script suffixes
  constexpr const char* LUA_STATE_SUFFIX = ".state.lua";
  constexpr const char* LUA_ENTITY_SUFFIX = ".entity.lua";
  constexpr const char* LUA_MAP_SUFFIX = ".map.lua";
  constexpr const char* LUA_ITEM_SUFFIX = ".item.lua";
  constexpr const char* LUA_ANIMATION_SUFFIX = ".animation.lua";

  // Default key bindings
  constexpr SDL_Scancode KEY_MOVE_UP = SDL_SCANCODE_W;
  constexpr SDL_Scancode KEY_MOVE_DOWN = SDL_SCANCODE_S;
  constexpr SDL_Scancode KEY_MOVE_LEFT = SDL_SCANCODE_A;
  constexpr SDL_Scancode KEY_MOVE_RIGHT = SDL_SCANCODE_D;
  constexpr SDL_Scancode KEY_ESCAPE = SDL_SCANCODE_ESCAPE;
  constexpr SDL_Scancode KEY_ENTER = SDL_SCANCODE_RETURN;
  constexpr SDL_Scancode KEY_ACTION_1 = SDL_SCANCODE_Z;
  constexpr SDL_Scancode KEY_ACTION_2 = SDL_SCANCODE_Y;
  constexpr SDL_Scancode KEY_ACTION_3 = SDL_SCANCODE_Q;
  constexpr SDL_Scancode KEY_ACTION_4 = SDL_SCANCODE_E;

  constexpr SDL_Scancode KEY_FUNC_HELP = SDL_SCANCODE_F1;
  constexpr SDL_Scancode KEY_FUNC_DEBUG = SDL_SCANCODE_F2;
  constexpr SDL_Scancode KEY_FUNC_FREEZE = SDL_SCANCODE_F3;
  constexpr SDL_Scancode KEY_FUNC_RESTART = SDL_SCANCODE_F4;
  constexpr SDL_Scancode KEY_FUNC_EXIT = SDL_SCANCODE_F5;
} 

#endif 
