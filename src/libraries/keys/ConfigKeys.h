#ifndef CONFIG_KEYS_H
#define CONFIG_KEYS_H

namespace Project::Libraries::Keys {
  //Keys for game engine configuration
  constexpr const char* CONFIG_FILE = "config.ini";
  constexpr const char* WINDOW_SECTION = "Window";
  constexpr const char* WINDOW_TITLE = "title";
  constexpr const char* WINDOW_WIDTH = "width";
  constexpr const char* WINDOW_HEIGHT = "height";
  constexpr const char* WINDOW_FULLSCREEN = "fullscreen";
  constexpr const char* WINDOW_SCALE_FACTORY = "scale_factor";
  constexpr const char* VIDEO_SECTION = "Video";
  constexpr const char* VIDEO_OPENGL = "opengl";
  constexpr const char* VIDEO_VSYNC = "vsync";
  constexpr const char* CAMERA_SECTION = "Camera";
  constexpr const char* CAMERA_TYPE = "type";
  constexpr const char* CURSOR_SECTION = "Cursor";
  constexpr const char* CURSOR_DEFAULT_PATH = "default_path";
  constexpr const char* CURSOR_H = "height";
  constexpr const char* CURSOR_W = "width";
  constexpr const char* FONT_SECTION = "Font";
  constexpr const char* FONT_DEFAULT_PATH = "default_path";
  constexpr const char* FONT_DEFAULT_SIZE = "default_size";
  constexpr const char* FONT_DEFAULT_COLOR = "default_color";
  constexpr const char* GAME_SECTION = "Game";
  constexpr const char* GAME_INITIAL_STATE = "initial_state";
  constexpr const char* LOGS_SECTION = "Logs";
  constexpr const char* LOGS_DIRECTORY = "directory";
  constexpr const char* LOG_FILE_PATH = "log_file_path";
  constexpr const char* LOG_LUA_LOG_FILE_PATH = "lua_log_file_path";
  constexpr const char* PATHS_SECTION = "Paths";
  constexpr const char* PATH_SCRIPTS = "scripts";
  constexpr const char* PATH_STYLES = "styles";
  constexpr const char* DEBUG_SECTION = "Debug";
  constexpr const char* DEBUG_TEXT_COLOR = "text_color";
  constexpr const char* AXIS_SECTION = "Axis";
  constexpr const char* AXIS_X_COLOR = "x_color";
  constexpr const char* AXIS_Y_COLOR = "y_color";
  constexpr const char* AXIS_Z_COLOR = "z_color";
  constexpr const char* POOLS_SECTION = "Pools";
  constexpr const char* POOL_ENTITY_MAX = "entity_max";
  constexpr const char* POOL_COMPONENT_MAX = "component_max";
}

#endif
