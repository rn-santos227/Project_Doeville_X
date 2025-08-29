#ifndef PATH_CONSTANTS_H
#define PATH_CONSTANTS_H

namespace Project::Libraries::Constants {
  constexpr const char* DEFAULT_CACHE_DIRECTORY = "cache";
  constexpr const char* DEFAULT_CSS_FILE = "default.css";
  constexpr const char* DEFAULT_CURSOR_PATH = "resources/system/cursor_default.png";
  constexpr const char* DEFAULT_ENTITY_SCRIPT_FOLDER = "scripts/entities/";
  constexpr const char* DEFAULT_FONT = "system";
  constexpr const char* DEFAULT_FONT_PATH = "resources/fonts/system.ttf";
  constexpr const char* DEFAULT_INITIAL_STATE = "MainMenu";
  constexpr const char* DEFAULT_LAYERS_SCRIPT_FOLDER = "scripts/layers/";
  constexpr const char* DEFAULT_LOGS_DIRECTORY = "resources/logs";
  constexpr const char* DEFAULT_LOG_FILE_PATH = "resources/logs/logs.txt";
  constexpr const char* DEFAULT_LUA_LOG_FILE_PATH = "resources/logs/lua_logs.txt";
  constexpr const char* DEFAULT_RESOURCES_FOLDER = "resources";
  constexpr const char* DEFAULT_SCRIPT_PATH = "scripts/";
  constexpr const char* DEFAULT_STATE_SCRIPT_FOLDER = "scripts/states/";
  constexpr const char* DEFAULT_STYLE_PATH = "resources/style";
  
  constexpr const char* HAND_CURSOR_PATH = "resources/system/cursor_hand.png";
  constexpr const char* TEXT_CURSOR_PATH = "resources/system/cursor_text.png";

  constexpr const char* LIGHT_PROBE_CACHE_FILE = "cache/light_probe.cache";
  constexpr const char* LUA_BYTECODE_CACHE_FILE = "cache/lua_bytecode.cache";
  constexpr const char* PATHFINDING_CACHE_FILE = "cache/pathfinding.cache";
  constexpr const char* SCRIPT_FUNCTION_CACHE_FILE = "cache/script_function_cache.cache";
  constexpr const char* SHADER_CACHE_FILE = "cache/shader.cache";
  constexpr const char* STYLE_CACHE_FILE = "cache/style.cache";
}

#endif
