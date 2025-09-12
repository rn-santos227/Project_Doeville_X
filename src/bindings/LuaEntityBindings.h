#ifndef LUA_ENTITY_BINDINGS_H
#define LUA_ENTITY_BINDINGS_H

extern "C" {
  #include <lua.h>
}

namespace Project::Bindings::LuaBindings {
  int lua_getCollidedEntity(lua_State* L);
  int lua_getEntityDetails(lua_State* L);
  int lua_getEntitySpeed(lua_State* L);
  int lua_getEntityVelocity(lua_State* L);
  int lua_getEntityRotation(lua_State* L);
  int lua_getNetworkPayload(lua_State* L);
  int lua_setNetworkConnection(lua_State* L);
  int lua_getVisibleEntities(lua_State* L);
  int lua_setEntityText(lua_State* L);
  int lua_setTimerActive(lua_State* L);
  int lua_setMeterActive(lua_State* L);
  int lua_setColor(lua_State* L);
  int lua_addNumericValue(lua_State* L);
  int lua_subtractNumericValue(lua_State* L);
  int lua_multiplyNumericValue(lua_State* L);
  int lua_divideNumericValue(lua_State* L);
  int lua_setNumericValue(lua_State* L);
  int lua_getNumericValue(lua_State* L);
  int lua_stopTimer(lua_State* L);
  int lua_brakeEntity(lua_State* L);
  int lua_turnLeft(lua_State* L);
  int lua_turnRight(lua_State* L);
}

#endif
