#ifndef LUA_FUNCTION_KEYS_H
#define LUA_FUNCTION_KEYS_H

namespace Project::Libraries::Keys {
  //lua function names
  constexpr const char* LUA_SET_ACTIVE_CAMERA = "setActiveCamera";
  constexpr const char* LUA_SET_BACKGROUND_COLOR = "setBackgroundColor";
  constexpr const char* LUA_SET_BACKGROUND_IMAGE = "setBackgroundImage";
  constexpr const char* LUA_GET_ENTITY_SPEED = "getEntitySpeed";
  constexpr const char* LUA_SET_ENTITY_SPEED = "setEntitySpeed";
  constexpr const char* LUA_GET_ENTITY_TEXT = "getEntityText";
  constexpr const char* LUA_SET_ENTITY_TEXT = "setEntityText";
  constexpr const char* LUA_SET_TIMER_ACTIVE = "setTimerActive";
  constexpr const char* LUA_SET_PLAYER_ENTITY = "setPlayerEntity";
  constexpr const char* LUA_ADD_NUMERIC_VALUE = "addNumericValue";
  constexpr const char* LUA_SET_NUMERIC_VALUE = "setNumericValue";
  constexpr const char* LUA_GET_NUMERIC_VALUE = "getNumericValue";
  constexpr const char* LUA_STOP_TIMER = "stopTimer";
  constexpr const char* LUA_ADD_ENTITY_TO_SEEDER = "addEntityToSeed";
  constexpr const char* LUA_BRAKE_ENTITY = "brakeEntity";
  constexpr const char* LUA_SPAWN_ENTITY = "spawnEntity";
  constexpr const char* LUA_DESTROY_ENTITY = "destroyEntity";
  constexpr const char* LUA_IS_ACTION_PRESSED = "isActionPressed";
  constexpr const char* LUA_START_ENTITY_SEEDER = "startEntitySeeder";
  constexpr const char* LUA_CHANGE_STATE = "changeState";
  constexpr const char* LUA_RESET_STATE = "resetState";
  constexpr const char* LUA_EXIT_GAME = "exitGame";
  constexpr const char* LUA_ON_TRIGGER = "onTrigger";
  constexpr const char* LUA_FUNC_PRINT = "print";
  constexpr const char* LUA_FUNC_SPAWN = "spawn";
}

#endif
