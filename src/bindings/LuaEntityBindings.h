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
}

#endif
