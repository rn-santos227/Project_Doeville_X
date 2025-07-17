#ifndef LUA_BINDINGS_H
#define LUA_BINDINGS_H

extern "C" {
  #include <lua.h>
}

namespace Project::Bindings {
  namespace LuaBindings {
    //Gamestate Bindings
    int lua_setActiveCamera(lua_State* L);
    int lua_setBackgroundImage(lua_State* L);
    int lua_setBackgroundColor(lua_State* L);
    int lua_spawnEntity(lua_State* L);
    int lua_changeState(lua_State* L);
    int lua_startEntitySeeder(lua_State* L);
  }
}

#endif
