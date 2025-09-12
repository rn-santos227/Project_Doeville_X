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
  int lua_setColor(lua_State* L
}

#endif
