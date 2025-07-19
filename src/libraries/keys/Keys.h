#ifndef KEYS_H
#define KEYS_H

#include <string>

namespace Project::Libraries::Keys {
  //Keys for game engine configuration
  constexpr const char* CONFIG_FILE = "config.ini";
  constexpr const char* WINDOW_SECTION = "Window";
  constexpr const char* WINDOW_TITLE = "title";
  constexpr const char* WINDOW_WIDTH = "width";
  constexpr const char* WINDOW_HEIGHT = "height";
  constexpr const char* WINDOW_FULLSCREEN = "fullscreen";
  constexpr const char* WINDOW_VSYNC = "vsync";
  constexpr const char* WINDOW_SCALE_FACTORY = "scale_factor";
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
  constexpr const char* DEBUG_SECTION = "Debug";
  constexpr const char* DEBUG_TEXT_COLOR = "text_color";
  constexpr const char* AXIS_SECTION = "Axis";
  constexpr const char* AXIS_X_COLOR = "x_color";
  constexpr const char* AXIS_Y_COLOR = "y_color";
  constexpr const char* AXIS_Z_COLOR = "z_color";

  //Keys for lua entities and components
  constexpr const char* ACCELERATION = "acceleration";
  constexpr const char* ACTIVE = "active";
  constexpr const char* ACTION = "action";
  constexpr const char* ACTIONS = "actions";
  constexpr const char* ALLOW_REVERT = "allow_revert";
  constexpr const char* BINDING = "binding";
  constexpr const char* BINDINGS = "bindings";
  constexpr const char* BOXES = "boxes"; 
  constexpr const char* CATEGORY = "category";  
  constexpr const char* CLASS = "class";
  constexpr const char* COLOR = "color";
  constexpr const char* COLOR_ALPHA = "color_alpha";
  constexpr const char* COLOR_HEX = "color_hex";
  constexpr const char* COMPONENT = "component";
  constexpr const char* COMPONENTS = "components";
  constexpr const char* DEFAULT = "default";
  constexpr const char* DELTA_TIME = "deltaTime";
  constexpr const char* DENSITY = "density";
  constexpr const char* ENABLED = "enabled";
  constexpr const char* FOLLOW_CAMERA = "follow_camera";
  constexpr const char* FONT_COLOR_HEX = "font_color_hex";
  constexpr const char* FONT_COLOR_ALPHA = "font_color_alpha";
  constexpr const char* FONT_SIZE = "font_size";
  constexpr const char* FONT_PATH = "font_path";
  constexpr const char* FORCE = "force";
  constexpr const char* FRICTION = "friction";
  constexpr const char* FLEXIBILITY = "flexibility";
  constexpr const char* FUNCTION = "function";
  constexpr const char* GLOBAL = "global";
  constexpr const char* GRAVITY = "gravity";
  constexpr const char* GRAVITY_SCALE = "gravity_scale";
  constexpr const char* GROUP = "group";
  constexpr const char* H = "h";
  constexpr const char* HEIGHT = "height";
  constexpr const char* ID = "id";
  constexpr const char* IMAGE = "image";
  constexpr const char* IMAGE_PATH = "image_path";
  constexpr const char* INITIALIZE = "initialize";
  constexpr const char* INTERACTABLE = "interactable";
  constexpr const char* KEY = "key";
  constexpr const char* KEYS = "keys";
  constexpr const char* KINEMATIC = "kinematic";
  constexpr const char* LAYER = "layer";
  constexpr const char* LAYER_CATEGORY = "layer_category";
  constexpr const char* LAYER_NAME = "layer_name";
  constexpr const char* LAYER_SCRIPTS = "layer_scripts";
  constexpr const char* MASS = "mass";
  constexpr const char* NAME = "name"; 
  constexpr const char* PLAYER = "player";
  constexpr const char* POSITION = "position";
  constexpr const char* RADIUS = "radius";
  constexpr const char* RECT = "rect";
  constexpr const char* RENDER = "render";
  constexpr const char* RESTITUTION = "restitution";
  constexpr const char* ROTATION = "rotation";
  constexpr const char* ROTATION_SPEED = "rotation_speed";
  constexpr const char* SCALE = "scale";
  constexpr const char* SECONDS = "seconds";
  constexpr const char* SIZE = "size";
  constexpr const char* SOLID = "solid";
  constexpr const char* SPEED = "speed";
  constexpr const char* SPIN = "spin";
  constexpr const char* STATE = "state";
  constexpr const char* STATE_NAME = "state_name";
  constexpr const char* STATE_CATEGORY = "state_category";
  constexpr const char* STATIC = "static";
  constexpr const char* SURFACE = "surface";
  constexpr const char* TAG = "tag";  
  constexpr const char* TEXT = "text";
  constexpr const char* TEXTURE = "texture";
  constexpr const char* TEXTURE_PATH = "texture_path";
  constexpr const char* TITLE = "title";
  constexpr const char* TIME = "time";
  constexpr const char* TYPE = "type";
  constexpr const char* UPDATE = "update";
  constexpr const char* USE_ACCELERATION = "use_acceleration";
  constexpr const char* VALUE = "value";
  constexpr const char* VISIBLE = "visible";
  constexpr const char* W = "w";
  constexpr const char* WIDTH = "width";
  constexpr const char* X = "x";
  constexpr const char* Y = "y";
  constexpr const char* Z = "z";
  constexpr const char* Z_INDEX = "z_index";
  constexpr const char* ZOOM = "zoom";
  constexpr const char* ZOOM_IN = "zoom_in";
  constexpr const char* ZOOM_OUT = "zoom_out";

  //Keys for lua game state
  constexpr const char* STATE_INITIALIZE = "initialize";
  constexpr const char* STATE_UPDATE = "update";
  constexpr const char* STATE_RENDER = "render";
  constexpr const char* STATE_ON_ATTACH = "onAttach";
  constexpr const char* STATE_ON_DETACH = "onDetach";
  constexpr const char* STATE_ON_ENTER = "onEnter";
  constexpr const char* STATE_ON_EXIT = "onExit";
  constexpr const char* STATE_HANDLE_INPUT = "handleInput";
  constexpr const char* STATE_PRINT = "print";

  //lua function names
  constexpr const char* LUA_SET_ACTIVE_CAMERA = "setActiveCamera";
  constexpr const char* LUA_SET_BACKGROUND_COLOR = "setBackgroundColor";
  constexpr const char* LUA_SET_BACKGROUND_IMAGE = "setBackgroundImage";
  constexpr const char* LUA_GET_ENTITY_SPEED = "getEntitySpeed";
  constexpr const char* LUA_SET_ENTITY_SPEED = "setEntitySpeed";
  constexpr const char* LUA_GET_ENTITY_TEXT = "getEntityText";
  constexpr const char* LUA_SET_ENTITY_TEXT = "setEntityText";
  constexpr const char* LUA_SET_PLAYER_ENTITY = "setPlayerEntity";
  constexpr const char* LUA_FUNC_PRINT = "print";
  constexpr const char* LUA_ADD_ENTITY_TO_SEEDER = "addEntityToSeed";
  constexpr const char* LUA_SPAWN_ENTITY = "spawnEntity";
  constexpr const char* LUA_START_ENTITY_SEEDER = "startEntitySeeder";
  constexpr const char* LUA_CHANGE_STATE = "changeState";
}

#endif
